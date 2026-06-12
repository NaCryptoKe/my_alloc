#include <stdio.h>
#include <assert.h>
#include "header.h"
#include "test.h"

int main() {
    // mmap path test
    printf("=== mmap path test ===\n");
    
    void *big = my_malloc(200 * 1024);  // 200KB — hits mmap path
    assert(big != NULL);
    printf("allocated 200KB\n");
    printf("--- /proc/self/maps after alloc ---\n");
    system("grep 'anon' /proc/self/maps");

    my_free(big);
    printf("--- /proc/self/maps after free ---\n");
    system("grep 'anon' /proc/self/maps");

    // confirm small allocations still work after mmap test
    printf("\n=== small alloc sanity check ===\n");
    int *arr = my_malloc(10 * sizeof(int));
    assert(arr != NULL);
    for (int i = 0; i < 10; i++) arr[i] = i;
    printf("arr[9] = %d\n", arr[9]);  // should print 9
    my_free(arr);

    // full stress test
    printf("\n");
    stress_test();

    return 0;
}
