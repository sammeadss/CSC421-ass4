#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

// Where all memory lives, including all chunks + usable memory
static int heap_pool[HEAP_SIZE];
// Linked lists of chunks
Chunk *free_lists[NUM_CLASSES];

// Get which linked list a chunk is in based off its size
int size_class(int n) {
    if (n <= 0) return 0;
    if (n >= NUM_CLASSES) return NUM_CLASSES - 1;
    return n - 1;
}

// Write size footer to the end of chunk
static void write_footer(Chunk *c) {
    // (c+1) to get to usable memory, then add size to get to end
    int *footer = (int *)(c + 1) + c->size;
    *footer = c->size; }

// Read size footer of chunk to the left of current chunk
static int left_footer(Chunk *c) {
    // (c-1) to get to size footer behind header fields, then dereference
    return *((int *)c - 1); }

void init(void) {
    int i;
    for (i = 0; i < NUM_CLASSES; i++) {
        free_lists[i] = NULL;
    }
    /* This is where all memory comes from, including usable memory:
    the heap pool cast as a chunk */
    Chunk *start = (Chunk *)heap_pool;

    start->prev = NULL;
    start->next = NULL;
    start->size = HEAP_SIZE - (int)(sizeof(Chunk) / sizeof(int));
    start->flag = CHUNK_FREE;
    write_footer(start);

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
    then add n to get the remainder chunk
    + 1 is for the footer */
    Chunk *remainder = (Chunk *)((int *)(c + 1) + n + 1);
    /* Original size - n taken - size for new header block
    sizeof(Chunk) for what's needed for a chunk;
    division for how much of a Chunk's space fit in each int? */
    remainder->size  = c->size - n - (int)(sizeof(Chunk) / sizeof(int));
    int cls          = size_class(remainder->size);
    remainder->prev  = NULL;
    remainder->next  = free_lists[cls];
    remainder->flag  = CHUNK_FREE;
    write_footer(remainder);

    c->size = n;
    write_footer(c);

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
                for a new chunk header + footer, there's still space left */
                if (c->size - n - header_words - 1 >= 1) {
                    split_chunk(c, n); }
		c->flag = CHUNK_USED;
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
    if (p == NULL) { return; }

    Chunk *c = (Chunk *)p - 1;
    c->flag = CHUNK_FREE;
    write_footer(c);

    int header_words = (int)(sizeof(Chunk) / sizeof(int));

    // MERGING

    // Grab chunk to the right, +1 is for footer
    Chunk *right = (Chunk *)((int *)(c + 1) + c->size + 1);
    /* heap_pool+HEAP_SIZE is the 1st address past the heap, so
    we confirm if right chunk points to insize the pool */
    if ((int *)right < heap_pool + HEAP_SIZE
        && right->flag == CHUNK_FREE) {
	/* Disconnect it from free list so nothing points to it
        as its own chunk*/
        unlink_chunk(right, size_class(right->size));
	/* Add the right chunk's entire memory space to original chunk's
	size so that original chunk treats right chunk as usable memory */
        c->size += right->size + header_words + 1;
        write_footer(c); }

    // Check left neighbour, does it point to outside the heap to the left?
    if ((int *)c > heap_pool) {
        int lsize = left_footer(c);
	// Start at left chunk's header
        Chunk *left = (Chunk *)((int *)c - lsize - header_words - 1);
        if (left->flag == CHUNK_FREE) {
	    // Same logic for disconnecting right chunk
            unlink_chunk(left, size_class(left->size));
            left->size += c->size + header_words + 1;
            write_footer(left);
            c = left; } }

    // Insert merged chunk into free list
    int cls = size_class(c->size);
    c->prev = NULL;
    c->next = free_lists[cls];
    if (free_lists[cls]) free_lists[cls]->prev = c;
    free_lists[cls] = c; }
