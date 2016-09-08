CC = gcc
CPPFLAGS = -std=c11 -Isrc/
CFLAGS = -Wall -Wextra -Wpedantic -Os
CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wformat=2
LDFLAGS =
LDLIBS = -lm -lncurses

## Enable debugging flags
#CPPSFLAGS += -UNDEBUG -DDEBUG
#CFLAGS += -Og -g

## Enable clang (I use both) ..
#CC = clang
#CFLAGS += -Weverything
#CFLAGS += -O0 -g

trolls: src/main.c src/troll.c src/location.c src/draw.c src/maze.c src/entity.c src/game.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

#draw.o: draw.c draw.h
	#$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

clean:
	rm -fv trolls *.o

.PHONY: clean all
