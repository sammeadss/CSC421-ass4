#ifndef HEAP_H
#define HEAP_H

#define HEAP_SIZE 65536
#define NUM_CLASSES 32

typedef struct Chunk {
    struct Chunk *prev;
    struct Chunk *next;
    int size;
} Chunk;

extern Chunk *free_lists[NUM_CLASSES];

void init(void);
void *newvec(int n);
void freevec(void *p);

int size_class(int n);

#endif
