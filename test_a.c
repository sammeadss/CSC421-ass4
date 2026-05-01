#include <stdio.h>
#include "heap.h"

int main(void) {
    init();

    int top = NUM_CLASSES - 1;

    if (free_lists[top] == NULL) {
        printf("FAIL: free_lists[%d] is NULL after init\n", top);
        return 1;
    }

    Chunk *c = free_lists[top];
    printf("PASS: free_lists[%d] = %p\n", top, (void *)c);
    printf("chunk size = %d words (expect %d)\n", c->size, HEAP_SIZE - (int)(sizeof(Chunk) / sizeof(int)));
    printf("prev = %p (expect NULL)\n", (void *)c->prev);
    printf("next = %p (expect NULL)\n", (void *)c->next);
    printf("\nsize_class() checks:\n");
    printf("size_class(1) = %d (expect 0)\n", size_class(1));
    printf("size_class(16) = %d (expect 15)\n", size_class(16));
    printf("size_class(32) = %d (expect 31)\n", size_class(32));
    printf("size_class(99) = %d (expect 31)\n", size_class(99));

    return 0;
}
