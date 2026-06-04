#include "test.h"
#include "header.h"

#define SIZE 100

void stress_test() {
    printf("=== STRESS TEST ===\n");

    void *ptrs[SIZE];

    // Allocating 100 blocks of differing sizes
    for(int i = 0; i < SIZE; i++) {
        ptrs[i] = my_sbrk_alloc((i % 8 + 1) * 8);   // sizes go 8, 16, 24 ...
        assert(ptrs[i] != NULL);
    }
    printf("100 allocations: OK\n");

    // free every other block
    for (int i = 0; i < SIZE; i += 2) {
        my_free(ptrs[i]);
        ptrs[i] = NULL;
    }
    printf("50 frees (EVERY OTHER): OK\n");

    // reallocating to the freed slots -- must always prints REUSED not NEW CARV
    for (int i = 0; i < SIZE; i+=2) {
        ptrs[i] = my_sbrk_alloc(8);     // making sure the new allocation is small enough to fit into any freed space
        assert(ptrs[i] != NULL);
    }
    printf("50 reallocations: OK\n");

    // free everything
    for(int i = 0; i < SIZE; i++) {
        my_free(ptrs[i]);
        ptrs[i] = NULL;
    }
    printf("FINAL FREE ALL: OK\n");

    // verifying if coalescing worked
    block_t *cur = heap_head;
    int consecutive_free = 0;
    while (cur != NULL) {
        if (cur -> size_and_flag & FREE_MASK) {
            consecutive_free++;
            assert(consecutive_free < 2);       // if coalescing appears 2 times in a row there is an error
        } else {
            consecutive_free = 0;
        }
        cur = cur -> next;
    }
    printf("No adjacent free blocks: OK\n");

    printf("=== STRESS TEST ENDED===\n");
}
