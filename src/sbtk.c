#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "header.h"
#include "test.h"

int main() {
    //stress_test();
    int *arr = my_malloc(5*sizeof(int));
    printf("%d\n", arr[0]);
    
    arr = realloc(arr, 20 * sizeof(int));
    return 0;
}
