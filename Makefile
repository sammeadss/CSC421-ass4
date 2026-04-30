CC = gcc
CFLAGS = -Wall -Wextra -g

all: test_a test_c

test_a: heap.c test_a.c
	$(CC) $(CFLAGS) -o test_a heap.c test_a.c

test_b: heap.c test_b.c
	$(CC) $(CFLAGS) -o test_b heap.c test_b.c

test_c: heap.c test_c.c
	$(CC) $(CFLAGS) -o test_c heap.c test_c.c

clean:
	rm -f test_a test_c
