#include "header.h"
#include <string.h>
#include <sys/mman.h>

block_t *heap_head = NULL;

void* my_malloc(size_t size) {

    // If the MMAP_THRESHOLD is passed, use mmap() instead of sbrk()
    if (size >= MMAP_THRESHOLD) {

        void *map = mmap (  NULL, sizeof(block_t) + size,
                            PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (map == MAP_FAILED)  return NULL;

        block_t *blk = (block_t *)map;
        
        // initializing the block
        blk->size_and_flag = size | MMAP_MASK;
        blk->next = NULL;
        blk->prev = NULL;

        return (void *)(blk + 1);
    }

    block_t *current = heap_head;
    
    // Check 1: checking if there is a free block that can fit the new size
    while (current != NULL) {
    
        size_t  block_size  =   current -> size_and_flag & SIZE_MASK;
        int     block_free  =   current -> size_and_flag & FREE_MASK;

        if (block_free && block_size >= size) {
            
            size_t  min_block   =   sizeof(block_t) + 8;    // it should atleasat hold 8 bits of memory and the header

            if (block_size >= size + min_block) {

                block_t *leftover   =   (block_t *)((char *)(current + 1) + size);

                leftover->size_and_flag =   (block_size - size - sizeof(block_t)) | FREE_MASK;
                leftover->next          =   current->next;
                leftover->prev          =   current;

                if (current->next) current->next->prev = leftover;  // making the next block make its prev to the leftover

                current->next = leftover;
                current->size_and_flag = size & SIZE_MASK;

            } else  current->size_and_flag &= ~FREE_MASK;

            return (void *)(current + 1);

        }

        current = current->next;

    }

    // Check 2: If there is no space to put in already existing one, create a new one
    void *additional = sbrk(sizeof(block_t) + size);

    if(additional == (void *)-1){
        
        perror("sbrk failed");
        exit(EXIT_FAILURE);

    }

    block_t *header_ptr = (block_t *) additional;     // This is telling the comiler to basically act as if this was a beginning of a block_t struct

    header_ptr->size_and_flag   =   size & SIZE_MASK;
    header_ptr->next            =   NULL;
    header_ptr->prev            =  NULL;

    if (heap_head == NULL)  heap_head = header_ptr;
    else {

        block_t *current    =   heap_head;

        while (current->next != NULL)
            current =   current->next;

        current->next       =   header_ptr;
        header_ptr->prev    =  current;
    
    }
    
    return (void *) (header_ptr + 1);
}

void* my_calloc(size_t count, size_t size){

    if (count == 0 || size == 0) return NULL;
    if (size > SIZE_MASK / count) return NULL;  // overflow guard

    size_t  total   =   count * size;
    void    *ptr    =   my_malloc(total);

    if (ptr == NULL) return NULL;

    memset(ptr, 0, total);      // zero out the data from ptr up to a size of total
    return ptr;

}

void* my_realloc(void* ptr, size_t new_size) {

    if (ptr == NULL) return my_malloc(new_size);    // if they write my_realloc(NULL, size), it is basically my_alloc(size);

    if (new_size == 0) {    // my_realloc(ptr, 0); is basically freeing the ptr allocation
        my_free(ptr);
        return NULL;
    }

    block_t *blk = ((block_t *) ptr) - 1;
    size_t current_size = blk->size_and_flag & SIZE_MASK;

    // for shrinking
    if (new_size <= current_size) return ptr;

    // if prev + current + next fits the new size
    int has_prev = blk->prev && (blk->prev->size_and_flag & FREE_MASK);
    int has_next = blk->next && (blk->next->size_and_flag & FREE_MASK);


    if (has_prev && has_next) {
        size_t prev_size = blk->prev->size_and_flag & SIZE_MASK;
        size_t next_size = blk->next->size_and_flag & SIZE_MASK;
        size_t total_size = prev_size + sizeof(block_t) + current_size + sizeof(block_t) + next_size;

        if (total_size >= new_size) {

            block_t *prev = blk->prev;
            memmove((void *)(prev + 1), ptr, current_size);
            prev->size_and_flag = new_size;
            prev->next = blk->next->next;
            
            if (prev->next) prev->next->prev = prev;
            return (void *)(prev + 1);
        
        }
    } else if (has_next) { // growing forwards in place

        size_t next_size = blk->next->size_and_flag & SIZE_MASK;
        
        if (current_size + sizeof(block_t) + next_size >= new_size) {   // the sizeof(block_t) is because the new block has a header
            blk->size_and_flag = new_size & FREE_MASK;  // updating the size
            blk->next = blk->next->next;

            if (blk->next) blk->next->prev = blk;
            return ptr;
        }

    } if (has_prev) { // growing backwards

        size_t prev_size = blk->prev->size_and_flag & SIZE_MASK;

        if (prev_size + sizeof(block_t) + current_size >= new_size) {
            block_t *prev = blk->prev;
            memmove((void *)(prev + 1), ptr, current_size);
            prev->size_and_flag = new_size;
            prev->next = blk->next;

            if (blk->next) blk->next->prev = prev;
            return (void *)(prev + 1);
        }

    }

    //if all reallocating failes, allocate new memory, copy from old and free the old
    void *new_ptr = my_malloc(new_size);

    if (new_ptr == NULL) return NULL;
    
    memcpy(new_ptr, ptr, current_size);
    
    my_free(ptr);
    
    return new_ptr;
}

void my_free(void *payload_ptr) {
    
    if (payload_ptr == NULL)    return;

    block_t *blk    =   ((block_t *)payload_ptr) - 1;

    if (blk->size_and_flag & MMAP_MASK) {   // checking if it was created by mmap()
        munmap(blk, sizeof(block_t) + (blk->size_and_flag & SIZE_MASK));
        return;
    }

    blk->size_and_flag   =   blk->size_and_flag | FREE_MASK;

    // forward coalescing
    if (blk -> next && my_block_is_free(blk->next)) {
        blk->size_and_flag += sizeof(block_t) + my_block_size(blk->next);
        blk->next = blk->next->next;
        
        if (blk->next) blk->next->prev = blk;
    }

    // backward coalescing
    if (blk->prev && my_block_is_free(blk->prev)) {
        blk->prev->size_and_flag += sizeof(block_t) + my_block_size(blk);
        blk->prev->next = blk->next;

        if(blk->next) blk->next->prev = blk->prev;
    }
}
