CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g -pthread
SRC_PARENT = parent.c
PROG_PARENT = parent

build:
	$(CC) $(CFLAGS) $(SRC_PARENT) -o $(PROG_PARENT)  -lrt && $(CC) $(CFLAGS) child1.c -o child1 -lrt && $(CC) $(CFLAGS) child2.c -o child2 -lrt

clean:
	rm $(PROG_PARENT)

