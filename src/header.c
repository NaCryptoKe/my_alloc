#include "header.h"

block_t *heap_head = NULL;

void* my_malloc(size_t size) {

    if (size >= 128 * 1024) {
        void *map = mmap (  NULL, sizeof(block_t) + size,
                            PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (map == MAP_FAILED)  return NULL;
        block_t *blk = (block_t *)map;
        blk->size_and_flag = size | MMAP_MASK;
        blk->next = NULL;
        blk->prev = NULL;

        return (void *)(blk + 1);
    }
    block_t *current = heap_head;
    while (current != NULL) {
        size_t  block_size  =   current -> size_and_flag & SIZE_MASK;
        int     block_free  =   current -> size_and_flag & FREE_MASK;

        if (block_free && block_size >= size) {
            size_t min_block = sizeof(block_t) + 8;

            if (block_size >= size + min_block) {
                block_t *leftover   = (block_t *)((char *)(current + 1) + size);
                leftover->size_and_flag = (block_size - size - sizeof(block_t)) | FREE_MASK;
                leftover->next = current->next;
                leftover->prev = current;

                if (current->next) current -> next -> prev = leftover;

                current->next = leftover;
                current->size_and_flag = size;
            } else {
                current -> size_and_flag &= ~FREE_MASK;
            }

            return (void *)(current + 1);
        }

        current = current->next;
    }
    void *additional = sbrk(sizeof(block_t) + size);
    if(additional == (void *)-1){
        perror("sbrk failed");
        exit(EXIT_FAILURE);
    }

    block_t *header_ptr = (block_t *) additional;     // This is telling the comiler to basically act as if this was a beginning of a block_t struct
    header_ptr->size_and_flag = size & SIZE_MASK;
    header_ptr->next = NULL;
    header_ptr->prev = NULL;

    if (heap_head == NULL) 
        heap_head = header_ptr;
    else {
        block_t *current    =   heap_head;

        while (current -> next != NULL)
            current = current -> next;

        current->next = header_ptr;
        header_ptr->prev = current;
    }
    
    return (void *) (header_ptr + 1);
}

void* my_calloc(size_t count, size_t size){
    if (count == 0 || size == 0) return NULL;
    if (size > SIZE_MASK / count) return NULL;  // overflow guard

    size_t total = count * size;
    void *ptr = my_malloc(total);
    if (ptr == NULL) return NULL;

    memset(ptr, 0, total);      // zero out the data from ptr up to a size of total
    return ptr;
}

void* my_realloc(void* ptr, size_t new_size) {
    if (ptr == NULL) return my_malloc(new_size);    // if they write my_realloc(NULL, size)
    if (new_size == 0) {
        my_free(ptr);
        return NULL;
    }

    block_t *blk = ((block_t *) ptr) - 1;
    size_t current_size = blk->size_and_flag & SIZE_MASK;

    // for shrinking
    if (new_size <= current_size) return ptr;

    // growing in place
    if (blk->next && (blk->next->size_and_flag & FREE_MASK)) {
        size_t next_size = blk->next->size_and_flag & SIZE_MASK;
        if (current_size + sizeof(block_t) + next_size >= new_size) {   // the sizeof(block_t) is because the new block has a header
            blk->size_and_flag = new_size;  // updating the size
            blk->next = blk->next->next;
            if (blk->next) blk->next->prev = blk;
            return ptr;
        }
    }
    // growing backwards
    if (blk->prev && (blk->prev->size_and_flag & FREE_MASK)) {
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
    }

    //allocate new memory, copy from old and free the old
    void *new_ptr = my_malloc(new_size);
    if (new_ptr == NULL) return NULL;
    memcpy(new_ptr, ptr, current_size);
    my_free(ptr);
    return new_ptr;
}

int size(void *payload_ptr) {
    block_t *header_ptr =   ((block_t *)payload_ptr) - 1;
    size_t total_size   =   header_ptr -> size_and_flag & SIZE_MASK;
    return total_size;
}

int is_free(void *payload_ptr) {
    block_t *header_ptr =   ((block_t *)payload_ptr) - 1;
    int is_free         =   header_ptr -> size_and_flag & FREE_MASK;   
    return is_free;
}

uintptr_t mem() {
    return (uintptr_t)sbrk(0);
}

void my_free(void *payload_ptr) {
    if (payload_ptr == NULL)    return;

    block_t *blk         =   ((block_t *)payload_ptr) - 1;

    if (blk->size_and_flag & MMAP_MASK) {
        munmap(blk, sizeof(block_t) + (blk->size_and_flag & SIZE_MASK));
        return;
    }

    blk->size_and_flag   =   blk->size_and_flag | FREE_MASK;
    // forward coalescing
    if (blk -> next && (blk->next->size_and_flag & FREE_MASK)) {
        blk->size_and_flag += sizeof(block_t) + (blk->next->size_and_flag & SIZE_MASK);
        blk->next = blk->next->next;
        if (blk->next) blk->next->prev = blk;
    }

    // backward coalescing
    if (blk->prev && (blk->prev->size_and_flag & FREE_MASK)) {
        blk->prev->size_and_flag += sizeof(block_t) + (blk->size_and_flag & SIZE_MASK);
        blk->prev->next = blk->next;
        if(blk->next) blk->next->prev = blk->prev;
    }
}
