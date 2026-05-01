#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

int main(void) {
    printf("Initializing heap...\n");
    init();

    printf("\n==== TEST 1: Basic Free ====\n");
    void *a = newvec(10);
    freevec(a);
    void *bPrime = newvec(10);

    if (a == bPrime) {
        printf("PASS: Free + reuse works\n");
    } else {
        printf("FAIL: Free broken\n");
    }

    printf("\n==== TEST 2: Right Coalescing ====\n");
    void *b = newvec(10);
    void *c = newvec(10);

    freevec(c); // free right first
    freevec(b); // should merge with c

    void *big1 = newvec(25); // requires merge

    if (big1 == b) {
        printf("PASS: Right coalescing works\n");
    } else {
        printf("FAIL: Right coalescing failed\n");
    }

    printf("\n==== TEST 3: Left Coalescing ====\n");
    void *d = newvec(10);
    void *e = newvec(10);

    freevec(d); // free left first
    freevec(e); // should merge with d

    void *big2 = newvec(25);

    if (big2 == d) {
        printf("PASS: Left coalescing works\n");
    } else {
        printf("FAIL: Left coalescing failed\n");
    }

    printf("\n==== TEST 4: Two-Sided Coalescing ====\n");
    void *f = newvec(10);
    void *g = newvec(10);
    void *h = newvec(10);

    freevec(f);
    freevec(h);
    freevec(g); // should merge all three

    void *big3 = newvec(40);

    if (big3 == f) {
        printf("PASS: Two-sided coalescing works\n");
    } else {
        printf("FAIL: Two-sided coalescing failed\n");
    }

    printf("\n==== TEST 5: Large Block Reuse ====\n");
    void *x = newvec(50);
    freevec(x);

    void *y = newvec(50);

    if (x == y) {
        printf("PASS: Large block reuse works\n");
    } else {
        printf("FAIL: Large block reuse failed\n");
    }

    printf("\n==== TEST 6: Fragmentation Resistance ====\n");
    void *p1 = newvec(5);
    void *p2 = newvec(5);
    void *p3 = newvec(5);
    void *p4 = newvec(5);

    freevec(p2);
    freevec(p4);
    freevec(p3); // should merge p2+p3+p4

    void *big4 = newvec(15);

    if (big4 == p2) {
        printf("PASS: Fragment merging works\n");
    } else {
        printf("FAIL: Fragment merging failed\n");
    }

    printf("\n==== TEST 7: freevec(NULL) ====\n");
    freevec(NULL);
    printf("PASS: freevec(NULL) did not crash\n");

    return 0;
}
