#ifndef HEADER_H
#define HEADER_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define FREE_MASK 0x1               // 0 is in use and 1 is free
#define MMAP_MASK 0x2
#define SIZE_MASK (~(size_t)0x3)    // clears both flag bits
#define MMAP_THRESHOLD 128 * 1024   // 128 KiB

typedef struct block {

    size_t size_and_flag;           // Stores both size and free flag to save space.
    struct block* prev;
    struct block* next;

    char data[];

} block_t;

extern block_t *heap_head;

void*   my_malloc   (size_t size);
void*   my_calloc   (size_t count, size_t size);
void*   my_realloc  (void* ptr, size_t new_size);
void    my_free     (void *payload_ptr);

/*
 * A helper function that returns block's size
*/
static inline size_t my_block_size(block_t *blk) {
    return blk->size_and_flag & SIZE_MASK;
}

/*
 * A helper function that returns block's free state
 * Returns 1 for free and 0 for not
*/
static inline int my_block_is_free(block_t *blk) {
    return blk->size_and_flag & FREE_MASK;
}

#endif
