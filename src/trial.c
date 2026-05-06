#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

int main() {
    size_t size = 16;

    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    
    memcpy(ptr, "Hello, World!", 14);
    printf("Data: %s\n", (char*)ptr);
    
    // Clean up (or basically calling free)
    munmap(ptr, size);
    return 0;
}
