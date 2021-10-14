CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g

build:
	$(CC) $(CFLAGS) parent.c -o parent && $(CC) $(CFLAGS) child1.c -o child1 && $(CC) $(CFLAGS) child2.c -o child2

clean:
	rm parent child1 child2

