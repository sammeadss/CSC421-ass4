#include <stdlib.h>
#include "heap.h"

static int heap_pool[HEAP_SIZE];
Chunk *free_lists[NUM_CLASSES];

int size_class(int n) {
    if (n <= 0) return 0;
    if (n >= NUM_CLASSES) return NUM_CLASSES - 1;
    return n - 1;
}

void init(void) {
    int i;
    for (i = 0; i < NUM_CLASSES; i++) {
        free_lists[i] = NULL;
    }
    Chunk *start = (Chunk *)heap_pool;
    start->prev = NULL;
    start->next = NULL;
    start->size = HEAP_SIZE - (int)(sizeof(Chunk) / sizeof(int));
    free_lists[size_class(start->size)] = start;
}

void *newvec(int n) {
    //TODO
    return 0;
}

void freevec(void *p) {
    //TODO
}
