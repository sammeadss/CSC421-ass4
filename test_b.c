#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

int main(void) {

	// Heap init
	init();

	// ERROR CHECKING TESTS
	void *zeroTest = newvec(0);
	if (zeroTest != NULL) { printf("FAIL: newvec is not NULL for 0.\n"); }
	void *negativeTest = newvec(-1);
	if (negativeTest != NULL) { printf("FAIL: newvec is not NULL for -1.\n"); }
	printf("PASS: Error checking tests.\n");

	// SPLIT-CHUNK TEST

	/* Heap is still mostly empty, so we want to test if split_chunk works
   	with the whole heap. */

	void *p = newvec(10);

	/* heap had HEAP_SIZE - 3 usable words, we took 10,
   	remainder should be HEAP_SIZE - 3 - 10 - 3 = HEAP_SIZE - 16 words
   	HEAP_SIZE - 3 for header - 10 we just took - 3 for new header */

	int expected_remainder = HEAP_SIZE
    	- (int)(sizeof(Chunk) / sizeof(int))
    	- 10
    	- (int)(sizeof(Chunk) / sizeof(int));

	int cls = size_class(expected_remainder);

	if (free_lists[cls] == NULL) { printf("FAIL: no remainder chunk found\n"); }
	else { printf("PASS: remainder chunk exists in class %d\n", cls); }

	// HEAP EXHAUSTION TEST
	init();
	void *big = newvec(HEAP_SIZE);
	if (big != NULL) { printf("FAIL: should have returned NULL\n"); }
	else { printf("PASS: oversized request returned NULL\n"); }
	void *small = newvec(1);
	if (small == NULL) { printf("FAIL: heap broken after failed alloc\n"); }
	else { printf("PASS: heap still usable after failed alloc\n"); }

	// LOOP TEST
	int sizes[10] = {1, 3, 7, 2, 15, 8, 4, 20, 6, 1};
	void *ptrs[10];
	for (int i = 0; i < 10; i++) {
    		ptrs[i] = newvec(sizes[i]);
    		if (ptrs[i] == NULL) { printf("FAIL: newvec returned NULL for size %d\n", sizes[i]); }
	}
	// Check if any of the pairs overlap
	for (int i = 0; i < 10; i++) {
    		for (int j = i + 1; j < 10; j++) {
        		int *end_i = (int *)ptrs[i] + sizes[i];
        		int *start_j = (int *)ptrs[j];
        		if (end_i > start_j && (int *)ptrs[i] < (int *)ptrs[j] + sizes[j]) {
            			printf("FAIL: ptrs[%d] and ptrs[%d] overlap\n", i, j);
			}
		}
	}
	printf("PASS: Loop tests.\n");

}
