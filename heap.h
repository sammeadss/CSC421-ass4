#ifndef HEAP_H
#define HEAP_H

#define HEAP_SIZE 65536 /* total pool size in words */
#define NUM_CLASSES 32 /* number of size-class free lists */

// Header stored at the start of every chunk in the pool
typedef struct Chunk {
    struct Chunk *prev; /* word 0 */
    struct Chunk *next; /* word 1 */
    int size; /* word 2 */
    int data[]; /* user data */
} Chunk;

// One head pointer per size class
extern Chunk *free_lists[NUM_CLASSES];

void init(void); /* initialize the heap */
void *newvec(int n); /* allocate n words */
void freevec(void *p); /* free a previously allocated block */

int size_class(int n); /* map size in words to a free list index */

#endif
