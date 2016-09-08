CPPFLAGS = -std=c11 -Isrc/
CFLAGS = -Wall -Wextra -Wpedantic -Os
CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wformat=2
CFLAGS += -Wmissing-prototypes -Wmissing-prototypes -Wredundant-decls
LDFLAGS =
LDLIBS = -lm -lncurses

## Enable debugging flags
CPPSFLAGS += -UNDEBUG -DDEBUG
CFLAGS += -Og -g

## Use clang
#CC = clang
#CFLAGS += -Weverything
#CFLAGS += -O0 -g

trolls: src/main.c src/troll.c src/location.c src/draw.c src/maze.c src/entity.c src/game.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

clean:
	rm -fv trolls

.PHONY: clean
