include config.mk

SRCS           :=
SUBDIRS        :=

include $(SRCDIR)/module.mk
-include $(EXT_SRCDIR)/module.mk

OBJS           := $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
SRCS           := $(addprefix $(SRCDIR)/,$(SRCS))

DEPS           := $(OBJS:.o=.d)
TMPS           := $(OBJS) $(OBJS:.o=.d)

CFLAGS         += -MD
CFLAGS         += $(addprefix -I./$(SRCDIR)/,$(SUBDIRS))

STATIC_LIB     := lib$(NAME).a
SHARED_LIB     := lib$(NAME).so.$(ABI_VERSION)
EXE            := $(NAME)$(EXEEXT)

all: $(EXE) $(SHARED_LIB) $(STATIC_LIB)

-include $(TESTDIR)/module.mk

-include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILE.c) -fPIC $(OUTPUT_OPTION) $<

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
	$(CC) $(CFLAGS) $(LINKFLAGS) -pie -rdynamic -o $@ $(OBJS)
clean-$(EXE):
	rm -f $(EXE)
else
$(EXE):
clean-$(EXE):
endif

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)
	@for dir in $(SUBDIRS); do    \
		mkdir -p $(OBJDIR)/$$dir; \
	done

mostlyclean:
	rm -f $(TMPS)

clean: mostlyclean clean-$(EXE) clean-$(STATIC_LIB) clean-$(SHARED_LIB)

distclean: clean
	rm -rf $(OBJDIR)

rebuild: distclean all

release: CFLAGS += -O3
release: all tests

.PHONY: all clean distclean rebuild release
