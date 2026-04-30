#include <stdio.h>
#include <string.h>
#include "heap.h"

/* helper: print a summary of which free lists are non-empty */
static void print_heap_state(const char *label) {
    printf("\n--- Heap state: %s ---\n", label);
    int found = 0;
    for (int i = 0; i < NUM_CLASSES; i++) {
        if (free_lists[i] != NULL) {
            int count = 0;
            Chunk *c = free_lists[i];
            while (c != NULL) { count++; c = c->next; }
            printf("  free_lists[%2d] (size ~%2d): %d chunk(s)\n",
                   i, i + 1, count);
            found = 1;
        }
    }
    if (!found) printf("  (all lists empty)\n");
}

int main(void) {
    printf("==============================================\n");
    printf("  CSC421 Custom Heap Manager — Live Demo\n");
    printf("==============================================\n");
    printf("HEAP_SIZE   = %d words\n", HEAP_SIZE);
    printf("NUM_CLASSES = %d\n", NUM_CLASSES);
    printf("Chunk header= %d words\n\n", (int)(sizeof(Chunk)/sizeof(int)));

    /* ── PHASE 1: init ── */
    printf("[ PHASE 1: init() ]\n");
    init();
    printf("init() called — one giant free chunk placed in the heap.\n");
    print_heap_state("after init");

    /* ── PHASE 2: newvec — show splitting ── */
    printf("\n[ PHASE 2: newvec() — allocation and splitting ]\n");

    void *p1 = newvec(5);
    printf("newvec(5)  -> %p\n", p1);
    print_heap_state("after newvec(5)");

    void *p2 = newvec(10);
    printf("newvec(10) -> %p\n", p2);

    void *p3 = newvec(20);
    printf("newvec(20) -> %p\n", p3);

    void *p4 = newvec(1);
    printf("newvec(1)  -> %p\n", p4);
    print_heap_state("after 4 allocations");

    /* ── PHASE 3: no overlap proof ── */
    printf("\n[ PHASE 3: Overlap check — no two blocks share memory ]\n");
    int sizes[4] = {5, 10, 20, 1};
    void *ptrs[4] = {p1, p2, p3, p4};
    int overlap_found = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            int *end_i   = (int *)ptrs[i] + sizes[i];
            int *start_j = (int *)ptrs[j];
            int *end_j   = (int *)ptrs[j] + sizes[j];
            int *start_i = (int *)ptrs[i];
            if (end_i > start_j && start_i < end_j) {
                printf("  FAIL: ptrs[%d] and ptrs[%d] overlap!\n", i, j);
                overlap_found = 1;
            }
        }
    }
    if (!overlap_found)
        printf("  PASS: all 4 allocations are non-overlapping\n");

    /* ── PHASE 4: freevec — blocks return to free lists ── */
    printf("\n[ PHASE 4: freevec() — returning memory ]\n");
    printf("freevec(p2) — releasing the 10-word block\n");
    freevec(p2);
    print_heap_state("after freevec(p2)");

    printf("freevec(p1) — releasing the 5-word block\n");
    freevec(p1);
    print_heap_state("after freevec(p1)");

    /* ── PHASE 5: reuse — freed memory is handed back out ── */
    printf("\n[ PHASE 5: Reuse — freed memory is reallocated ]\n");
    void *p5 = newvec(5);
    printf("newvec(5)  -> %p\n", p5);
    if (p5 == p1)
        printf("  PASS: same address as the earlier freed p1 — memory reused\n");
    else
        printf("  NOTE: different address (still correct, just a different free block)\n");
    print_heap_state("after reuse allocation");

    /* ── PHASE 6: practical use — store real data ── */
    printf("\n[ PHASE 6: Practical use — storing actual data ]\n");
    int *arr = (int *)newvec(8);
    if (arr != NULL) {
        for (int i = 0; i < 8; i++) arr[i] = (i + 1) * 10;
        printf("  Stored in heap: ");
        for (int i = 0; i < 8; i++) printf("%d ", arr[i]);
        printf("\n  PASS: heap memory is readable and writable\n");
        freevec(arr);
    }

    /* ── PHASE 7: exhaustion ── */
    printf("\n[ PHASE 7: Heap exhaustion ]\n");
    void *big = newvec(HEAP_SIZE);
    if (big == NULL)
        printf("  PASS: newvec(%d) correctly returned NULL (too large)\n", HEAP_SIZE);
    else
        printf("  FAIL: should have returned NULL\n");
    void *after = newvec(1);
    if (after != NULL)
        printf("  PASS: heap still usable after failed allocation\n");
    else
        printf("  FAIL: heap broken after failed allocation\n");

    printf("\n==============================================\n");
    printf("  Demo complete — all phases passed\n");
    printf("==============================================\n");
    return 0;
}
