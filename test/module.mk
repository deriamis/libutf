CUR_LIST_DIR     := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

-include $(EXT_SRCDIR)/unity.mk
LIST_DIR     := $(CUR_LIST_DIR)

TEST_SRCS    := test_decode.c

TEST_SRCS    := $(addprefix $(LIST_DIR)/,$(TEST_SRCS))
TEST_OBJS    := $(addprefix $(OBJDIR)/,$(TEST_SRCS:.c=.o))
TEST_OBJDIR  := $(addprefix $(OBJDIR)/,$(LIST_DIR))
TEST_BINS    := $(addprefix $(OBJDIR)/,$(TEST_SRCS:.c=$(EXEEXT)))

TEST_DEPS    := $(addprefix $(LIST_DIR)/,util.c)
TESTDEP_OBJS := $(addprefix $(OBJDIR)/,$(TEST_DEPS:.c=.o))

$(TEST_OBJDIR): | $(OBJDIR)
	@mkdir -p $(OBJDIR)/$(TESTDIR)

$(TEST_OBJS): CPPFLAGS += -I$(UNITY_DIR)
$(TEST_OBJS): $(TEST_SRCS) | $(TEST_OBJDIR)
	$(COMPILE.c) -fPIC $(OUTPUT_OPTION) $<

$(TESTDEP_OBJS): $(TEST_DEPS) | $(TEST_OBJDIR)
	$(COMPILE.c) -fPIC $(OUTPUT_OPTION) $<

$(TEST_BINS): LINKFLAGS += -L. -l$(NAME)
$(TEST_BINS): $(TEST_OBJS) | $(TESTDEP_OBJS) $(UNITY_OBJS)
	$(CC) $(CFLAGS) $(LINKFLAGS) -pie -rdynamic -o $@ $< \
		$(TESTDEP_OBJS) $(UNITY_OBJS)

tests: DEPS := $(TEST_OBJS:.o=.d) $(TESTDEP_OBJS:.o=.d) $(UNITY_OBJS:.o=.d)
tests: all $(TEST_BINS)

retest: distclean tests

recheck: distclean check

check: tests
	@for test_bin in $(TEST_BINS); do \
		$(VALGRIND_EXEC) $$test_bin;  \
	done

debug: CFLAGS += -O1 -g -g3 -ggdb
debug: LDFLAGS += -g
debug: recheck

.PHONY: debug tests retest check recheck
