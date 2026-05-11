typedef struct block {
    size_t size;
    size_t is_free;

    struct block* next;
    struct block* prev;

    char data[];
} block_t;

