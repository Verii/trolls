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

all: bin/path_queue bin/path_bheap

bin/path_queue: benchmark_path.c $(SOURCES)
	$(CC) -DBENCH_PATH_QUEUE $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

bin/path_bheap: benchmark_path.c $(SOURCES)
	$(CC) -DBENCH_PATH_BHEAP $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

run:
	/usr/bin/time -v ./bin/path_queue
	/usr/bin/time -v ./bin/path_bheap

clean:
	rm -fv ./bin/path_queue ./bin/path_bheap

.PHONY: clean
