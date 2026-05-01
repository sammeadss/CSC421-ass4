#include <stdio.h>
#include "heap.h"

/* helper: check all values in a block match expected */
static int check_block(int *p, int size, int expected, const char *label) {
    for (int i = 0; i < size; i++) {
        if (p[i] != expected) {
            printf("FAIL [%s]: word %d = %d, expected %d\n",
                   label, i, p[i], expected);
            return 0; } }
    printf("PASS [%s]: all %d words = %d\n", label, size, expected);
    return 1; }

/* ── Test 1: basic write and read ── */
static void test_basic_write_read(void) {
    printf("\n=== Test 1: basic write and read ===\n");
    init();

    int *p = (int *)newvec(8);
    if (p == NULL) { printf("FAIL: newvec returned NULL\n"); return; }

    /* write distinct values to every word */
    for (int i = 0; i < 8; i++)
        p[i] = (i + 1) * 10;   /* 10, 20, 30, 40, 50, 60, 70, 80 */

    /* read them back */
    int pass = 1;
    for (int i = 0; i < 8; i++) {
        if (p[i] != (i + 1) * 10) {
            printf("FAIL: p[%d] = %d, expected %d\n", i, p[i], (i+1)*10);
            pass = 0; } }
    if (pass)
        printf("PASS: all 8 words written and read correctly\n");

    freevec(p); }

/* ── Test 2: two blocks don't interfere with each other ── */
static void test_two_blocks_independent(void) {
    printf("\n=== Test 2: two blocks are independent ===\n");
    init();

    int *a = (int *)newvec(10);
    int *b = (int *)newvec(10);
    if (a == NULL || b == NULL) { printf("FAIL: newvec returned NULL\n"); return; }

    /* fill a with 1s and b with 2s */
    for (int i = 0; i < 10; i++) { a[i] = 1; b[i] = 2; }

    /* writing to b should not affect a, and vice versa */
    check_block(a, 10, 1, "a unchanged after writing b");
    check_block(b, 10, 2, "b unchanged after writing a");

    /* now overwrite a with 99s and confirm b is still 2s */
    for (int i = 0; i < 10; i++) a[i] = 99;
    check_block(b, 10, 2, "b still intact after overwriting a");

    freevec(a);
    freevec(b); }

/* ── Test 3: freed memory can be reallocated and written again ── */
static void test_realloc_after_free(void) {
    printf("\n=== Test 3: reallocate and overwrite freed memory ===\n");
    init();

    int *p1 = (int *)newvec(6);
    if (p1 == NULL) { printf("FAIL: newvec returned NULL\n"); return; }

    /* write a pattern */
    for (int i = 0; i < 6; i++) p1[i] = 0xDEAD;
    freevec(p1);

    /* reallocate same size — may get same block back */
    int *p2 = (int *)newvec(6);
    if (p2 == NULL) { printf("FAIL: newvec returned NULL after free\n"); return; }

    /* write a new pattern — must not crash or corrupt */
    for (int i = 0; i < 6; i++) p2[i] = 0xBEEF;
    check_block(p2, 6, 0xBEEF, "reallocated block writable");

    if (p2 == p1)
        printf("NOTE: same address reused — freevec recycled correctly\n");
    else
        printf("NOTE: different address — still correct\n");

    freevec(p2); }

/* ── Test 4: multiple blocks, free in reverse, reallocate ── */
static void test_reverse_free(void) {
    printf("\n=== Test 4: allocate 5 blocks, free in reverse order ===\n");
    init();

    int *ptrs[5];
    int sizes[5] = {3, 7, 2, 10, 5};

    /* allocate and fill each block with its index */
    for (int i = 0; i < 5; i++) {
        ptrs[i] = (int *)newvec(sizes[i]);
        if (ptrs[i] == NULL) {
            printf("FAIL: newvec returned NULL for block %d\n", i);
            return; }
        for (int j = 0; j < sizes[i]; j++)
            ptrs[i][j] = i; }

    /* verify all blocks still hold correct values before freeing */
    int pass = 1;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < sizes[i]; j++)
            if (ptrs[i][j] != i) {
                printf("FAIL: block %d word %d corrupted\n", i, j);
                pass = 0; }
    if (pass)
        printf("PASS: all 5 blocks intact before freeing\n");

    /* free in reverse order */
    for (int i = 4; i >= 0; i--)
        freevec(ptrs[i]);

    /* reallocate and confirm writable */
    int *fresh = (int *)newvec(10);
    if (fresh == NULL) {
        printf("FAIL: newvec returned NULL after all frees\n"); return; }
    for (int i = 0; i < 10; i++) fresh[i] = 42;
    check_block(fresh, 10, 42, "fresh allocation after reverse free");
    freevec(fresh); }

/* ── Test 5: write to large block, spot check boundaries ── */
static void test_large_block_boundaries(void) {
    printf("\n=== Test 5: large block boundary check ===\n");
    init();

    int n = 500;
    int *p = (int *)newvec(n);
    if (p == NULL) { printf("FAIL: newvec returned NULL\n"); return; }

    /* write sentinel values at first, middle, and last word */
    p[0]     = 0xAAAAAAAA;
    p[n/2]   = 0x55555555;
    p[n - 1] = 0xBBBBBBBB;

    /* fill the rest */
    for (int i = 1; i < n - 1; i++)
        if (i != n/2) p[i] = i;

    /* verify sentinels survived */
    int pass = 1;
    if (p[0]     != 0xAAAAAAAA) { printf("FAIL: first word corrupted\n");  pass = 0; }
    if (p[n/2]   != 0x55555555) { printf("FAIL: middle word corrupted\n"); pass = 0; }
    if (p[n - 1] != 0xBBBBBBBB) { printf("FAIL: last word corrupted\n");  pass = 0; }
    if (pass)
        printf("PASS: boundary words intact across %d-word block\n", n);

    freevec(p); }

/* ── Test 6: allocate, free one, allocate again, others unaffected ── */
static void test_middle_free_no_corruption(void) {
    printf("\n=== Test 6: free middle block, neighbours unaffected ===\n");
    init();

    int *left   = (int *)newvec(5);
    int *middle = (int *)newvec(5);
    int *right  = (int *)newvec(5);
    if (!left || !middle || !right) {
        printf("FAIL: newvec returned NULL\n"); return; }

    for (int i = 0; i < 5; i++) { left[i] = 11; middle[i] = 22; right[i] = 33; }

    /* free the middle block */
    freevec(middle);

    /* left and right should be completely unaffected */
    check_block(left,  5, 11, "left after middle freed");
    check_block(right, 5, 33, "right after middle freed");

    freevec(left);
    freevec(right); }

int main(void) {
    printf("==============================================\n");
    printf("  Heap usable memory tests\n");
    printf("==============================================\n");

    test_basic_write_read();
    test_two_blocks_independent();
    test_realloc_after_free();
    test_reverse_free();
    test_large_block_boundaries();
    test_middle_free_no_corruption();

    printf("\n==============================================\n");
    printf("  All tests complete\n");
    printf("==============================================\n");
    return 0; }
