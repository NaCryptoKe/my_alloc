#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "header.h"

int main() {
    printf("=== sbrk / brk ===\n");
    uintptr_t start = mem();
    printf("Initial program break: %p\n", start);

    int *a = my_sbrk_alloc(40);
    int *b = my_sbrk_alloc(64);
    int *c = my_sbrk_alloc(32);

    my_free(b);
    block_t *h = ((block_t *)a) - 1;
    printf ("Size of a: %zu\n", h->size_and_flag & SIZE_MASK);

    my_free(a);
    h = ((block_t *)a) - 1;
    printf ("Size of a: %zu\n", h->size_and_flag & SIZE_MASK);
    
    return 0;
}
