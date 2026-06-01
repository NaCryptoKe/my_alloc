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

void* my_sbrk_alloc(int size);
uintptr_t mem();

#endif
