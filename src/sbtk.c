#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "header.c"

int main() {
    printf("=== sbrk / brk ===\n");
    uintptr_t start = (uintptr_t)sbrk(0);
    printf("Initial program break: %p\n", start);

    size_t size = 1024;

    block_t header = {size, 0, NULL, NULL};           // declaring the header

    void *additional = sbrk(sizeof(block_t) + size);    // give me 1KiB and size of the header block in bytes
    if(additional == (void*)-1) {
        printf("Error");
        exit(EXIT_FAILURE);
    }
    uintptr_t end = (uintptr_t)sbrk(0);
    int x = (int)(end - start - sizeof(block_t));
    printf("Current program break: %p, size of header: %d, size: %d\t%x\n", end, sizeof(block_t), x, x);

    additional = sbrk(sizeof(block_t) + size);    // give me 1KiB and size of the header block in bytes                                                                                             
    if(additional == (void*)-1) {                                                                                                                                                                         
        printf("Error");                                                                                                                                                                                  
        exit(EXIT_FAILURE);                                                                                                                                                                               
    }                                                                                                                                                                                                     
    end = (uintptr_t)sbrk(0);                                                                                                                                                                   
    x = (int)(end - start - (2 * sizeof(block_t)));                                                                                                                                                         
    printf("Current program break: %p, size of header: %d, size: %d\t%x\n", end, sizeof(block_t), x, x); 

    additional = sbrk(sizeof(block_t) + size);    // give me 1KiB and size of the header block in bytes                                                                                             
    if(additional == (void*)-1) {                                                                                                                                                                         
        printf("Error");                                                                                                                                                                                  
        exit(EXIT_FAILURE);                                                                                                                                                                               
    }                                                                                                                                                                                                     
    end = (uintptr_t)sbrk(0);                                                                                                                                                                   
    x = (int)(end - start - (3 * sizeof(block_t)));                                                                                                                                                         
    printf("Current program break: %p, size of header: %d, size: %d\t%x\n", end, sizeof(block_t), x, x);
}
