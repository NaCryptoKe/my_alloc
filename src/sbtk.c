#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "header.h"

int main() {
    printf("=== sbrk / brk ===\n");
    uintptr_t start = mem();
    printf("Initial program break: %p\n", start);

    int *arr = my_sbrk_alloc(10 * sizeof(int));

    block_t *header_ptr =   ((block_t *)arr) - 1;
    size_t total_size   =   header_ptr -> size_and_flag & SIZE_MASK;
    int is_free         =   header_ptr -> size_and_flag & FREE_MASK;   

    uintptr_t end = mem();
    printf("Current program break: %p, size of header: %d, size: %d, is_free: %d\n", end, sizeof(block_t), total_size, is_free);

    return 0;
}
