SOURCES = src/main.c \
          src/troll.c \
          src/location.c \
          src/draw.c \
          src/maze.c \
          src/entity.c \
          src/game.c \
          src/path.c

CPPFLAGS = -std=c11 -Iinclude
CFLAGS = -Wall -Wextra -Wpedantic -Os
CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wformat=2
CFLAGS += -Wmissing-prototypes -Wmissing-prototypes -Wredundant-decls
LDFLAGS =
LDLIBS = -lm -lncurses

## Enable debugging flags
CPPFLAGS += -UNDEBUG -DDEBUG
CFLAGS += -Og -ggdb3

## Use clang
#CC = clang
#CFLAGS += -Weverything
#CFLAGS += -O0 -g

trolls: $(SOURCES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

clean:
	rm -fv trolls

.PHONY: clean
