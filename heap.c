#include <stdlib.h>
#include <stdio.h>
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

// Helper methods for newvec

// Unlinking a chunk from its free chunk list
static void unlink_chunk(Chunk *c, int cls) {
    if (c->prev) { c->prev->next = c->next; }
    // If now taken chunk was the head of the list,
    else { free_lists[cls] = c->next; }
    if (c->next) { c->next->prev = c->prev; }
}

// Splitting a chunk if it's too large for the user
static void split_chunk(Chunk *c, int n) {
    /* (c + 1) is the actual memory past the header
    then add n to get the remainder chunk */
    Chunk *remainder = (Chunk *)((int *)(c + 1) + n);
    /* Original size - n taken - size for new header block
    sizeof(Chunk) for what's needed for a chunk;
    division for how much of a Chunk's space fit in each int? */
    remainder->size  = c->size - n - (int)(sizeof(Chunk) / sizeof(int));
    c->size          = n;
    int cls          = size_class(remainder->size);
    remainder->prev  = NULL;
    remainder->next  = free_lists[cls];
    if (free_lists[cls]) free_lists[cls]->prev = remainder;
    free_lists[cls]  = remainder;
}

// newvec for getting memory
void *newvec(int n) {
    // Error checking
    if (n <= 0) {
     printf("Given size <= 0\n");
     return NULL;
    }
    int cls = size_class(n);
    /* sizeof(Chunk) for what's needed for a chunk;
    division for how much of a Chunk's space fit in each int? */
    int header_words = (int)(sizeof(Chunk) / sizeof(int));
    // Iterate through all free chunk lists
    while (cls < NUM_CLASSES) {
        Chunk *c = free_lists[cls];
        // Iterate through each free chunk list
        while (c != NULL) {
            if (c->size >= n) {
                unlink_chunk(c, cls);
                /* If, after taking away the n memory needed and space
                for a new chunk header, there's still space left */
                if (c->size - n - header_words >= 1)
                    split_chunk(c, n);
                // Return memory after the header
                return (void *)(c + 1);
            }
            c = c->next;
        }
        cls++;
    }
    // If no free list contains any free chunk of n size
    printf("Heap exhausted.\n");
    return NULL;
}

void freevec(void *p) {
    if (p == NULL) {
        return;
    }
    
    // Get the chunk pointer, which is right before the given memory
    Chunk *c = (Chunk *)p - 1;
    
    int cls = size_class(c->size);
    
    // Put the chunk back at the head of the free list
    c->prev = NULL;
    c->next = free_lists[cls];
    if (free_lists[cls] != NULL) {
        free_lists[cls]->prev = c;
    }
    free_lists[cls] = c;
}
