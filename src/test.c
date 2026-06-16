#include "test.h"
#include "header.h"
#include <string.h>

#define SMALL_SIZE 250
#define LARGE_SIZE 200

void stress_test() {
    printf("\n===============================================\n");
    printf("         STARTING ADVANCED STRESS TEST         \n");
    printf("===============================================\n");

    // Trackers for our allocated memory pointers
    void *small_ptrs[SMALL_SIZE];
    void *large_ptrs[LARGE_SIZE];

    // TEST 1: Mixed Allocations & Data Integrity (Writable Checks)
    printf("[1/5] Allocating mixed sbrk and mmap blocks...\n");
    
    // Allocate small blocks via sbrk path
    for (int i = 0; i < SMALL_SIZE; i++) {
        size_t bytes = ((i % 4) + 1) * 32; // Sizes: 32, 64, 96, 128 bytes
        small_ptrs[i] = my_malloc(bytes);
        assert(small_ptrs[i] != NULL);
        
        // Write pattern to ensure memory is writable and tracking is clean
        memset(small_ptrs[i], 0xAA, bytes); 
    }
    printf(" -> %d Small blocks (sbrk path) allocated and verified.\n", SMALL_SIZE);

    // Allocate large blocks passing the MMAP_THRESHOLD (128 KB)
    for (int i = 0; i < LARGE_SIZE; i++) {
        size_t bytes = MMAP_THRESHOLD + (i * 4096); // Pages above threshold
        large_ptrs[i] = my_malloc(bytes);
        assert(large_ptrs[i] != NULL);
        
        // Write pattern to ensure physical pages are mapped properly
        memset(large_ptrs[i], 0xBB, bytes);
    }
    printf(" -> %d Large blocks (mmap path) allocated and verified.\n", LARGE_SIZE);


    // TEST 2: Alternate Frees & Fragmentation Simulation
    printf("\n\n[2/5] Simulating fragmentation (Freeing every second block)...\n");
    
    // Free every other small block to create alternate "free holes"
    for (int i = 0; i < SMALL_SIZE; i += 2) {
        my_free(small_ptrs[i]);
        small_ptrs[i] = NULL;
    }
    
    // Free every other large block to test munmap tracking independently
    for (int i = 0; i < LARGE_SIZE; i += 2) {
        my_free(large_ptrs[i]);
        large_ptrs[i] = NULL;
    }
    printf(" -> Fragmentation layout completed successfully.\n");


    // TEST 3: Block Splitting & Reallocation Reuse
    printf("\n\n[3/5] Testing reallocation reuse and block splitting...\n");
    
    // Request tiny blocks that should fit cleanly into the freed small slots
    for (int i = 0; i < SMALL_SIZE; i += 2) {
        small_ptrs[i] = my_malloc(8); // Tiny 8-byte request to force splitting
        assert(small_ptrs[i] != NULL);
        memset(small_ptrs[i], 0xCC, 8);
    }
    printf(" -> Small block splitting / reuse tracking: OK\n");


    // TEST 4: Massive Heap Cleanup
    printf("\n\n[4/5] Cleaning up all active memory blocks...\n");
    
    // Empty out remaining small blocks
    for (int i = 0; i < SMALL_SIZE; i++) {
        if (small_ptrs[i] != NULL) {
            my_free(small_ptrs[i]);
            small_ptrs[i] = NULL;
        }
    }

    // Empty out remaining large mmap blocks
    for (int i = 0; i < LARGE_SIZE; i++) {
        if (large_ptrs[i] != NULL) {
            my_free(large_ptrs[i]);
            large_ptrs[i] = NULL;
        }
    }
    printf(" -> All allocated test memory successfully returned.\n");


    // TEST 5: Heap Validation & Coalescing Check
    printf("\n\n[5/5] Validating heap state and verifying coalescing...\n");

    block_t *cur = heap_head;
    int consecutive_free = 0;
    int total_free_blocks = 0;


    printf("Size of block: %ld, is free: %d\n", my_block_size(cur), my_block_is_free(cur));

    while (cur != NULL) {
        // Double-check flags directly safely
        if (my_block_is_free(cur)) {
            consecutive_free++;
            total_free_blocks++;
            
            // CRITICAL ASSERTION: If coalescing works properly, two free blocks 
            // can NEVER stand adjacent to one another in the list.
            if (consecutive_free >= 2) {
                printf("\n [ERROR] Coalescing Failure: Detected adjacent free blocks at %p!\n", (void*)cur);
                assert(consecutive_free < 2);
            }
        } else {
            consecutive_free = 0; // Reset counter when an allocated block is seen
        }
        cur = cur->next;
    }

    printf(" -> Heap structure evaluation: OK (No adjacent free blocks found).\n");
    printf("All total free blocks: %d\n", total_free_blocks);
    printf("\n===============================================\n");
    printf("       STRESS TEST COMPLETED SUCCESSFULLY      \n");
    printf("===============================================\n");
}