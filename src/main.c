#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

int main() {
    // ============================
    // 1. Using sbrk() — the old way
    // ============================
    // sbrk(increment) adjusts the "program break" (end of data segment).
    // The return value is the PREVIOUS break address, i.e., the start of
    // the newly allocated region if increment > 0.

    printf("=== sbrk / brk ===\n");
    // Get current break.
    void *initial_break = sbrk(0);
    printf("Initial program break: %p\n", initial_break);

    // Extend the data segment by exactly 16 bytes.
    // In practice, the kernel may round up to page boundary, but sbrk
    // still gives you a pointer that's usable for 16 bytes (the extra
    // space is just wasted / could be used later).
    void *brk_region = sbrk(16);
    if (brk_region == (void*)-1) {
        perror("sbrk");
        exit(EXIT_FAILURE);
    }
    printf("Allocated 16 bytes at:  %p (via sbrk)\n", brk_region);
    printf("New program break:       %p\n", sbrk(0));

    // Write the 16 bytes to prove it's usable.
    memset(brk_region, 0xAB, 16);
    printf("First byte: 0x%X\n", ((unsigned char*)brk_region)[0]);

    // Note: you cannot free this memory individually — sbrk is a contiguous
    // arena. Memory allocators (malloc) usually reserve a large chunk with
    // sbrk and manage it internally, only releasing back via negative sbrk
    // when the entire top of the heap is free.

    // ============================
    // 2. Using mmap() — modern, flexible
    // ============================
    printf("\n=== mmap ===\n");
    // mmap can map a new anonymous (not backed by a file) memory region.
    // We request exactly 16 bytes, but the kernel will give an entire page
    // (4096 bytes typically). The extra bytes are accessible but wasteful.
    void *mmap_region = mmap(NULL,                     // let kernel choose address
                             16,                       // requested size
                             PROT_READ | PROT_WRITE,   // permissions
                             MAP_PRIVATE | MAP_ANONYMOUS, // not shared, no file
                             -1,                       // fd (ignored)
                             0);                       // offset (ignored)
    if (mmap_region == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    printf("Mapped 16 bytes at:  %p (via mmap)\n", mmap_region);

    // Use the memory.
    memset(mmap_region, 0xCD, 16);
    printf("First byte: 0x%X\n", ((unsigned char*)mmap_region)[0]);

    // You can later return this memory to the OS independently using munmap.
    munmap(mmap_region, 16);

    // ============================
    // 3. Summary for a custom malloc
    // ============================
    printf("\n=== Key differences for allocator design ===\n");
    printf("- sbrk gives a contiguous heap that can only grow/shrink at the end.\n");
    printf("  Good for a classic dlmalloc-style allocator (small blocks).\n");
    printf("- mmap gives independent chunks that can be freed in any order.\n");
    printf("  Essential for large blocks and per-thread arenas.\n");
    printf("- Both round up to page size, so for tiny (16-byte) requests,\n");
    printf("  your allocator should carve them out of a larger chunk,\n");
    printf("  not call the kernel for each one.\n");

    return 0;
}
