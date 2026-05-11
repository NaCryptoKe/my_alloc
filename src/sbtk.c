#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "header.c"

int main() {
    printf("=== sbrk / brk ===\n");
    uintptr_t initial_break = (uintptr_t)sbrk(0);
    printf("Initial program break: %p\n", initial_break);

    char arr[10] = {0};
    block_t header = {1024, 0, NULL, NULL};           // declaring the header
    void *additional = sbrk(sizeof(block_t) + 1024);    // give me 1KiB and size of the header block in bytes
    if(additional == (void*)-1) {
        printf("Error");
        exit(EXIT_FAILURE);
    }
    uintptr_t end = (uintptr_t)sbrk(0);
    int x = (int)(end - initial_break);
    printf("Current program break: %p, size of header: %d, size: %d\t%x", initial_break, sizeof(block_t), x, x);
}
