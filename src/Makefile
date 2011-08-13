ifndef IPC
IPC=socket
endif

SRCDIR=../src
OBJDIR=../bin
TARGET=$(OBJDIR)/sim
FOLDERS=. $(CHILD_FOLDERS)
CHILD_FOLDERS=$(shell find . -type d -mindepth 1)
ALL_OBJS=$(addprefix $(OBJDIR)/,$(foreach dir,$(FOLDERS),$(patsubst %.c, %.o, $(wildcard $(dir)/*.c)) $(patsubst %.asm, %-asm.o, $(wildcard $(dir)/*.asm))))
INCLUDES=$(addprefix $(SRCDIR)/,$(foreach dir,$(FOLDERS), $(wildcard $(dir)/*.h)))

OBJS=$(filter-out $(OBJDIR)/./ipc/%.o, $(ALL_OBJS)) $(OBJDIR)/./ipc/$(IPC).o

CFLAGS=-I$(SRCDIR) -pedantic -std=c99 -fstrict-aliasing -Wall -Wextra -Wshadow -Wcast-qual \
	-Wwrite-strings -Wpointer-arith -Wcast-align -Wmissing-prototypes \
	-Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline \
	-Wstrict-prototypes -Wunreachable-code 

.SUFFIXES:
.SUFFIXES: .c .o .h

.PHONY: debug release clean all prepare

.DEFAULT: $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@
%.c:

$(TARGET): prepare $(OBJS) $(INCLUDES)
# The wire, and filter-out is a hack to make sure the loader is at the start of the endfile, so it can load!
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

all: release

# Debug flags
debug: override CFLAGS += -O0 -g
debug: $(TARGET)

# Release flags
release: override CFLAGS += -O3
release: $(TARGET)

clean: 
	-rm $(OBJS)
ifneq ($(strip $(CHILD_FOLDERS)),)
	-cd $(OBJDIR) && rm -rf $(CHILD_FOLDERS)
endif
	-rm $(TARGET)

prepare:
	mkdir -p $(OBJDIR)
ifneq ($(strip $(CHILD_FOLDERS)),)
	cd $(OBJDIR) && mkdir -p $(CHILD_FOLDERS)
endif
