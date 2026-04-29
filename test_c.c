#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

int main(void) {
    // Heap init
    init();

    // Allocate some chunks
    int sizes[10] = {1, 3, 7, 2, 15, 8, 4, 20, 6, 1};
    void *ptrs[10];
    for (int i = 0; i < 10; i++) {
        ptrs[i] = newvec(sizes[i]);
    }

    // FREE TEST
    for (int i = 0; i < 10; i++) {
        freevec(ptrs[i]);
    }

    // Check if they were successfully freed
    // Sizes were {1, 3, 7, 2, 15, 8, 4, 20, 6, 1}
    if (free_lists[size_class(1)] != NULL && 
        free_lists[size_class(3)] != NULL && 
        free_lists[size_class(20)] != NULL) {
        printf("PASS: Free tests passed. Memory chunks successfully added back to free lists.\n");
    } else {
        printf("FAIL: Free tests failed to add memory chunks to free_lists.\n");
    }

    // NULL POINTER TEST
    printf("Testing freevec(NULL)...\n");
    freevec(NULL); // Should not crash
    printf("PASS: freevec(NULL) did not crash.\n");

    // REUSABILITY TEST
    void *first_ptr = newvec(50);
    freevec(first_ptr);
    void *second_ptr = newvec(50);
    
    if (first_ptr == second_ptr) {
        printf("PASS: Memory reusability test passed. Same pointer returned.\n");
    } else {
        printf("FAIL: Memory reusability test failed. Pointer %p != %p\n", first_ptr, second_ptr);
    }

    return 0;
}
