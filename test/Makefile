SOURCES = ../src/troll.c \
          ../src/location.c \
          ../src/maze.c \
          ../src/entity.c \
          ../src/game.c

CPPFLAGS = -std=c11 -I../include
CFLAGS = -Wall -Wextra -Wpedantic -Os
CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wformat=2
CFLAGS += -Wmissing-prototypes -Wmissing-prototypes -Wredundant-decls
LDFLAGS =
LDLIBS = -lm

## Enable debugging flags
CPPSFLAGS += -UNDEBUG -DDEBUG
CFLAGS += -Og -ggdb3

## Use clang
#CC = clang
#CFLAGS += -Weverything
#CFLAGS += -O0 -g

benchmark-queue: benchmark_path-queue.c $(SOURCES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

run:
	/usr/bin/time -v ./benchmark-queue

clean:
	rm -fv benchmark-queue

.PHONY: clean
