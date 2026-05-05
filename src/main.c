#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int         fd;
    char        *addr;
    off_t       offset, pa_offset;
    size_t      length;
    ssize_t     s;
    struct stat sb;

    if (argc < 3 || argc > 4) {
        fprintf(stderr, "%s file offset [length]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
        err(EXIT_FAILURE, "open");

    if (fstat(fd, &sb) == -1)       // to obtain file size
        err(EXIT_FAILURE, "fstat");

    offset = atoi(argv[2]);
    pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);     // offset for mmap() must be page aligned
    
    if (offset >= sb.st_size) {
        fprintf(stderr, "offset is past end file\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 4) { 
        length = atoi(argv[3]);
        if (offset + length > sb.st_size)
                   length = sb.st_size - offset;
                       /* Can't display bytes past end of file */

           } else {    /* No length arg ==> display to end of file */
               length = sb.st_size - offset;
           }

           addr = mmap(NULL, length + offset - pa_offset, PROT_READ,
                       MAP_PRIVATE, fd, pa_offset);
           if (addr == MAP_FAILED)
               err(EXIT_FAILURE, "mmap");

           s = write(STDOUT_FILENO, addr + offset - pa_offset, length);
           if (s != length) {
               if (s == -1)
                   err(EXIT_FAILURE, "write");

               fprintf(stderr, "partial write");
               exit(EXIT_FAILURE);
           }

           munmap(addr, length + offset - pa_offset);
           close(fd);

           exit(EXIT_SUCCESS);
}
