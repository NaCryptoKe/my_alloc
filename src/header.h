#ifndef HEADER_H
#define HEADER_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>         // for uintptr_t
#include <unistd.h>         // for sbrk()
#include <stdio.h>          // for perror()

#define FREE_MASK 0x1
#define SIZE_MASK (~(size_t)0x1)    // clears the lowest bit
                                    
typedef struct block {
    size_t size_and_flag;       // Stores both size and free flag to save space.
    struct block* prev;
    struct block* next;

    char data[];
} block_t;

extern block_t *heap_head;

void* my_malloc(size_t size);
void* my_calloc(size_t count, size_t size);
int size(void *payload_ptr);
int is_free(void *payload_ptr);
uintptr_t mem();
void my_free(void *payload_ptr);

#endif
