malloc, calloc, realloc, or free which are stored on the secondary memory rather than the stack which is on the RAM.
The OS uses brk/mmap to do this.

The heap is just a linked strucutre of chunks that has metadata (size, free/used) and user data area(what the user gets) 
```
[metadata][data][metadata][data][metadata][freespace]...
```

## Malloc

Malloc is basically getting freechunk big enough for the task in hand, then returns pointer to the data section and no metadata will be provided. `It will not clear the data that was there beforehand, it will give you the garbage that was there`

## Calloc

Calloc is malloc but *cleaned*. Allocates memory and sets it to zero.

## Realloc

It's basicaly trying to resize the block.

If there is space next to the block currently being used it expands to that, which is easy. But if no space was available next to it, it allocates a new block then copies the old data then frees the old block.

Basically its making a bigger malloc, then copying it to that then freeing the old one.

## free

Is making the block to be marked reusable. Memory won't return to OS immediately, it will be marked `free` then added to a `free list`; then future `malloc` calls may reuse it.

| Function | Core job                      |
| -------- | ----------------------------- |
| malloc   | Allocate uninitialized memory |
| calloc   | Allocate + zero it            |
| realloc  | Resize allocation             |
| free     | Return it to allocator        |

# mmap()

``` c
#include <sys/mman.h>

void *mmap(size_t length;
                  void addr[length], size_t length, int prot, int flags,
                  int fd, off_t offset);
int munmap(size_t length;
                  void addr[length], size_t length);
```

Creates a new mapping in the virtual memory.

Starting address is passed to `addr`, and `length` tells exactly how much size it should be (it should be above 0), fd is for file descriptor, prot for memory protection.

I'll be using MEM_ANONYMOUS for flag cause I just want raw memory address. 

and **`munmap`** are for freeing the memory acquired by mmap().

mmap() returns a pointer if successful and -1 if not (which means it doesn't have NULL). 
