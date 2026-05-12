#ifndef HEADER_H
#define HEADER_H

typedef block {
    char is_free;
    size_t size;

    struct block* prev;
    struct block* next;

    char data[];
}

void* my_sbrk_alloc(int size);

#endif
