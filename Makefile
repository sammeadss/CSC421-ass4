CC = gcc
CFLAGS = -Wall -Wextra -g

all: test_a

test_a: heap.c test_a.c
	$(CC) $(CFLAGS) -o test_a heap.c test_a.c

clean:
	rm -f test_a
