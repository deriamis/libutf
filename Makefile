include config.mk

SRCS           :=
TEST_SRCS      :=
SUBDIRS        :=
TESTS          :=

include $(SRCDIR)/module.mk
include $(TESTDIR)/module.mk

OBJS           := $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
TEST_OBJS      := $(addprefix $(OBJDIR)/$(TESTDIR)/,$(TEST_SRCS:.c=.o))
TEST_BINS      := $(addprefix $(OBJDIR)/$(TESTDIR)/,$(TESTS))

SRCS           := $(addprefix $(SRCDIR)/,$(SRCS))
TEST_SRCS      := $(addprefix $(TESTDIR)/,$(TEST_SRCS))

DEPS           := $(OBJS:.o=.d)
TMPS           := $(OBJS) $(OBJS:.o=.d)

CFLAGS         += -MD
CFLAGS         += $(addprefix -I./$(SRCDIR)/,$(SUBDIRS))

STATIC_LIB     := lib$(NAME).a
SHARED_LIB     := lib$(NAME).so.$(ABI_VERSION)
EXE            := $(NAME)

all: $(EXE) $(SHARED_LIB) $(STATIC_LIB)

debug: CFLAGS += -g -g3 -ggdb
debug: LDFLAGS += -g
debug: rebuild

tests: DEPS := $(TEST_OBJS:.o=.d)
tests: all $(TEST_BINS)

-include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(OBJDIR)/$(TESTDIR)/unity.o: CPPFLAGS += -I$(UNITY_DIR)
$(OBJDIR)/$(TESTDIR)/unity.o: $(UNITY_DIR)/unity.c | $(OBJDIR)/$(TESTDIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(OBJDIR)/$(TESTDIR)/%.o: CPPFLAGS += -I$(UNITY_DIR)
$(OBJDIR)/$(TESTDIR)/%.o: $(TESTDIR)/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

ifneq ($(filter shared,$(LIBRARY_TYPE)),)
$(SHARED_LIB): $(OBJS)
	$(LD) $(LDFLAGS) -shared -soname $(SHARED_LIB) -o $@ $(OBJS)
clean-$(SHARED_LIB):
	rm -f $(SHARED_LIB)
else
$(SHARED_LIB):
clean-$(SHARED_LIB):
endif

ifneq ($(filter shared,$(LIBRARY_TYPE)),)
$(STATIC_LIB): $(OBJS)
	$(AR) rcs $@ $(OBJS)
clean-$(STATIC_LIB):
	rm -f $(STATIC_LIB)
else
$(STATIC_LIB):
clean-$(STATIC_LIB):
endif

ifneq ($(LIBRARY_TYPE),$(filter $(LIBRARY_TYPE),shared static))
$(EXE):	$(OBJS)
	$(CC) $(CFLAGS) $(LINKFLAGS) -rdynamic -o $@ $(OBJS)
clean-$(EXE):
	rm -f $(EXE)
else
$(EXE):
clean-$(EXE):
endif

$(TEST_BINS): LINKFLAGS += -L. -l$(NAME)
$(TEST_BINS): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(LINKFLAGS) -rdynamic -o $@ $< $(OBJDIR)/$(TESTDIR)/unity.o

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)
	@for dir in $(SUBDIRS); do    \
		mkdir -p $(OBJDIR)/$$dir; \
	done

$(TEST_OBJS): $(OBJDIR)/$(TESTDIR)/unity.o
$(TEST_OBJS): | $(OBJDIR)/$(TESTDIR)

$(OBJDIR)/$(TESTDIR): $(OBJDIR)
	@mkdir -p $(OBJDIR)/$(TESTDIR)

mostlyclean:
	rm -f $(TMPS)

clean: mostlyclean clean-$(EXE) clean-$(STATIC_LIB) clean-$(SHARED_LIB)

distclean: clean
	rm -rf $(OBJDIR)

rebuild: distclean all

check: tests

.PHONY: all clean distclean rebuild tests check
