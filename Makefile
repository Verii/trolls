CC = gcc
CPPFLAGS = -std=c11
CFLAGS = -Wall -Wextra -Wpedantic -Os -DDEBUG
CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wformat=2
LDFLAGS =
LDLIBS = -lm -lncurses -levent_core

## Enable debugging flags
#CPPSFLAGS += -UNDEBUG -DDEBUG
#CFLAGS += -Og -g

## Enable clang (I use both) ..
CC = clang
CFLAGS += -Weverything

trolls: src/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

#draw.o: draw.c draw.h
	#$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

clean:
	rm -fv trolls *.o

.PHONY: clean all
