#include "header.h"

void* my_sbrk_alloc(size_t size) {

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

            printf("REUSED\n");
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
    
    printf("NEW CARV\n");
    return (void *) (header_ptr + 1);
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
    if (payload_ptr == NULL)
        return;

    block_t *header_ptr         =   ((block_t *)payload_ptr) - 1;
    header_ptr->size_and_flag   =   header_ptr->size_and_flag | FREE_MASK;
}
