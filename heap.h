#ifndef HEAP_H
#define HEAP_H

#define HEAP_SIZE 65536
#define NUM_CLASSES 32

// Alternating recognizable bit patterns
#define CHUNK_FREE 0xAAAAAAAA
#define CHUNK_USED 0x55555555

typedef struct Chunk {
    int flag; // For whether it's free or not
    struct Chunk *prev;
    struct Chunk *next;
    int size;
    /* Size at end not stored in struct; it lives in
    a computed position we can read/write to directly */
} Chunk;

extern Chunk *free_lists[NUM_CLASSES];

void init(void);
void *newvec(int n);
void freevec(void *p);

int size_class(int n);

#endif
