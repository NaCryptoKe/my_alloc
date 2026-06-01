#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "header.h"

int main() {
    printf("=== sbrk / brk ===\n");
    uintptr_t start = mem();
    printf("Initial program break: %p\n", start);

    int *arr = my_sbrk_alloc(100 * sizeof(int));

    uintptr_t end = mem();
    printf("Current program break: %p, size of header: %d, size: %d, is_free: %d\n", end, sizeof(block_t), size(arr), is_free(arr));

    for (int i = 0; i < 10; i++) {
        arr[i] = i * 10;
    }
    for (int i = 0; i < 10; i++) {
        printf ("%d\n", arr[i]);
    }
    my_free(arr);

    uintptr_t last = mem();
    printf("Initial program break: %p\n", last);
    int *arr1 = my_sbrk_alloc(4 * sizeof(int));
    int *arr2 = my_sbrk_alloc(3 * sizeof(int));
    int *arr3 = my_sbrk_alloc(2 * sizeof(int));
    int *arr4 = my_sbrk_alloc(1 * sizeof(int));

    for (int i = 0; i < 10; i++) {
        printf ("%d\n", arr1[i]);
    }
    return 0;
}
