CC ?= gcc
CFLAGS   = -Wall -mtune=native -no-pie `sdl2-config --cflags`
LIBS  = `sdl2-config --libs`

SRC = main.c

OBJDIR := objdir
OBJS := $(addprefix $(OBJDIR)/,$(SRC:.c=.o))

.PHONY=all clean

all: cgol

$(OBJDIR)/%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

cgol: $(OBJS)
	$(CC) -o $(@F) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	rm -f cgol