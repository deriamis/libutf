NAME           := utf
VERSION        := 0.1
ABI_VERSION    := 0
LIBRARY_TYPE   := shared static

SRCDIR         := src
OBJDIR         := build
TESTDIR        := test
UNITY_DIR      := third_party/Unity/src

CC             := clang
LD             := lld

INCLUDES       := include
TARGET_MACHINE := x86_64-linux-gnu

LIBDIRS        := /lib                             \
				  /usr/lib                         \
				  /usr/local/lib                   \
				  /lib/$(TARGET_MACHINE)           \
				  /usr/lib/$(TARGET_MACHINE)       \
				  /usr/local/lib/$(TARGET_MACHINE)
EXTRA_LIBDIRS  :=

LIBS           := c m
EXTRA_LIBS     :=

INCFLAGS       := $(addprefix -I,$(INCLUDES))
LIBFLAGS       := $(addprefix -L,$(LIBDIRS)) $(addprefix -l,$(LIBS))
EXTRA_LIBFLAGS := $(addprefix -L,$(EXTRA_LIBDIRS)) $(addprefix -l,$(EXTRA_LIBS))

CPPFLAGS       := -std=c89                               \
				  -pedantic                              \
				  -pedantic-errors                       \
				  -fgnuc-version=0                       \
                  -D_XOPEN_SOURCE=700                    \
                  -D_GNU_SOURCE                          \
                  -D_FORTIFY_SOURCE=2                    \
				  $(INCFLAGS)

CFLAGS         := -O3                                    \
                  -Wall                                  \
                  -Wextra                                \
                  -Wcast-align                           \
                  -Wunused                               \
				  -Wno-unused-function                   \
                  -Wpedantic                             \
                  -Wconversion                           \
                  -Wsign-conversion                      \
                  -Wnull-dereference                     \
                  -Wdouble-promotion                     \
                  -Wformat=2                             \
                  -Werror                                \
                  -fvisibility=hidden                    \
                  -fstack-protector-strong               \
                  -ffunction-sections                    \
                  -fdata-sections                        \
                  -fstrict-aliasing

LINKFLAGS      := -fuse-ld=$(subst ld.,,$(notdir $(LD))) \
				  -Wl,--no-as-needed                     \
                  -Wl,--fatal-warnings                   \
                  -Wl,--gdb-index                        \
                  -Wl,--build-id                         \
                  -Wl,--hash-style=gnu                   \
                  -Wl,--gc-sections                      \
                  -Wl,-z,noexecstack                     \
                  -Wl,--warn-execstack                   \
                  -Wl,-z,relro                           \
                  -Wl,-z,origin                          \
                  -Wl,--enable-new-dtags                 \
                  -Wl,-z,defs                            \
				  $(EXTRA_LIBFLAGS)

LDFLAGS        := --no-as-needed                         \
                  --fatal-warnings                       \
                  --gdb-index                            \
                  --build-id                             \
                  --hash-style=gnu                       \
                  --gc-sections                          \
                  -z noexecstack                         \
                  --warn-execstack                       \
                  -z relro                               \
                  -z origin                              \
                  --enable-new-dtags                     \
                  -z defs                                \
    	          $(LIBFLAGS)                            \
				  $(EXTRA_LIBFLAGS)
