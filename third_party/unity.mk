LIST_DIR     := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

UNITY_DIR      := $(addprefix $(LIST_DIR)/,Unity/src)
UNITY_SRCS     := $(UNITY_DIR)/unity.c
UNITY_OBJS     := $(addprefix $(OBJDIR)/,$(UNITY_SRCS:.c=.o))
UNITY_OBJDIR   := $(addprefix $(OBJDIR)/,$(UNITY_DIR))

$(UNITY_OBJDIR): | $(OBJDIR)
	@mkdir -p $(UNITY_OBJDIR)

$(UNITY_OBJS): CPPFLAGS += -I$(UNITY_DIR)
$(UNITY_OBJDIR)/%.o: $(UNITY_DIR)/%.c | $(UNITY_OBJDIR)
	$(COMPILE.c) -fPIC $(OUTPUT_OPTION) $^

