#include "header.h"

void* my_sbrk_alloc(int size) {
    void *additional = sbrk(sizeof(block_t) + size);

    if(additional == (void *)-1){
        perror("sbrk failed");
        exit(EXIT_FAILURE);
    }

    block_t *header_ptr = (block_t *) additional;     // This is telling the comiler to basically act as if this was a beginning of a block_t struct

    header_ptr->size_and_flag = size | FREE_MASK;
    header_ptr->next = NULL;
    header_ptr->prev = NULL;

    return (void *) (header_ptr + 1);
}

uintptr_t mem() {
    return (uintptr_t)sbrk(0);
}
