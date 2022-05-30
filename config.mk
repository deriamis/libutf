NAME           := utf
VERSION        := 0.1
ABI_VERSION    := 0
LIBRARY_TYPE   := shared static

TOPDIR         := $(PWD)
INCLUDES       := include
LIBS           :=
SRCDIR         := src
OBJDIR         := build

INCFLAGS       := $(addprefix -I,$(INCLUDES))
LIBFLAGS       := $(foreach TMP,$(LIBS),-L$(TMP))

CC             := clang
CPPFLAGS       := -std=c89 -pedantic -pedantic-errors -fgnuc-version=0 $(INCFLAGS)
CFLAGS         := -O3                                   \
                  -Wall                                 \
                  -Wextra                               \
                  -Wcast-align                          \
                  -Wunused                              \
                  -Wpedantic                            \
                  -Wconversion                          \
                  -Wsign-conversion                     \
                  -Wnull-dereference                    \
                  -Wdouble-promotion                    \
                  -Wformat=2                            \
                  -Werror                               \
                  -fvisibility=hidden                   \
                  -fstack-protector-strong              \
                  -ffunction-sections                   \
                  -fdata-sections                       \
                  -fstrict-aliasing                     \
    	          $(LIBFLAGS)
LDFLAGS        := -fuse-ld=lld-13                       \
				  -Wl,--no-as-needed                    \
                  -rdynamic                             \
                  -Wl,--fatal-warnings                  \
                  -Wl,--gdb-index                       \
                  -Wl,--build-id                        \
                  -Wl,--hash-style=gnu                  \
                  -Wl,--gc-sections,--print-gc-sections \
                  -Wl,-z,noexecstack                    \
                  -Wl,--warn-execstack                  \
                  -Wl,-z,relro                          \
                  -Wl,-z,origin                         \
                  -Wl,--enable-new-dtags                \
                  -Wl,-z,defs                           \
                  -D_XOPEN_SOURCE=700                   \
                  -D_GNU_SOURCE                         \
                  -D_FORTIFY_SOURCE=2
