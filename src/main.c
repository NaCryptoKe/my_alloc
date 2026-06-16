#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "header.h"
#include "test.h"
#include "benchmark.h"

#define ITERATIONS 1000

int main(void) {
    // Standard Glibc memory tracking arrays
    double *glibc_m_latencies = malloc(ITERATIONS * sizeof(double));
    double *glibc_f_latencies = malloc(ITERATIONS * sizeof(double));
    double *glibc_c_latencies = malloc(ITERATIONS * sizeof(double));
    double *glibc_r_latencies = malloc(ITERATIONS * sizeof(double));
    void **ptrs = malloc(ITERATIONS * sizeof(void *));
    
    // Custom Allocator tracking arrays
    double *my_m_latencies = malloc(ITERATIONS * sizeof(double));
    double *my_f_latencies = malloc(ITERATIONS * sizeof(double));
    double *my_c_latencies = malloc(ITERATIONS * sizeof(double));
    double *my_r_latencies = malloc(ITERATIONS * sizeof(double));
    void **my_ptrs = malloc(ITERATIONS * sizeof(void *));

    bench_timer_t timer;

    // --- WARMUP STAGE ---
    // Warm up the heap and cache layers so iteration 0 doesn't distort the graphs completely
    // Because everytime I run it, it spicked at iteration 0. I think it was because of the cache data missmatch
    for(int w = 0; w < 10; w++) {
        void *g = malloc(32); 
        free(g);

        void *m = my_malloc(32); 
        my_free(m);
    }

    // PHASE 1: Standard Glibc Benchmarks
    
    // 1. Glibc Malloc
    for (int i = 0; i < ITERATIONS; i++) {
        start_timer(&timer);
        ptrs[i] = malloc(sizeof(int) * 4); // 16 bytes
        glibc_m_latencies[i] = stop_timer(&timer);
        prevent_optimization(ptrs[i]);
        if (!ptrs[i]) { perror("Glibc Malloc failed"); exit(EXIT_FAILURE); }
    }

    // 2. Glibc Realloc (Modifying sizes to trigger combinations)
    for (int i = 0; i < ITERATIONS; i++) {
        start_timer(&timer);
        ptrs[i] = realloc(ptrs[i], sizeof(int) * 8); // Resize up to 32 bytes
        glibc_r_latencies[i] = stop_timer(&timer);
        prevent_optimization(ptrs[i]);
    }

    // 3. Glibc Free
    for (int i = 0; i < ITERATIONS; i++) {
        start_timer(&timer);
        free(ptrs[i]);
        glibc_f_latencies[i] = stop_timer(&timer);
    }

    // 4. Glibc Calloc
    for (int i = 0; i < ITERATIONS; i++) {
        start_timer(&timer);
        ptrs[i] = calloc(4, sizeof(int));
        glibc_c_latencies[i] = stop_timer(&timer);
        prevent_optimization(ptrs[i]);
    }
    // Cleanup calloc blocks cleanly
    for (int i = 0; i < ITERATIONS; i++) { free(ptrs[i]); }

    // PHASE 2: Your Custom Allocator Benchmarks

    // 1. Custom Malloc
    for (int i = 0; i < ITERATIONS; i++) {
        start_timer(&timer);
        my_ptrs[i] = my_malloc(sizeof(int) * 4);
        my_m_latencies[i] = stop_timer(&timer);
        prevent_optimization(my_ptrs[i]);
        if (!my_ptrs[i]) { perror("Custom Malloc failed"); exit(EXIT_FAILURE); }
    }

    // 2. Custom Realloc
    for (int i = 0; i < ITERATIONS; i++) {
        start_timer(&timer);
        my_ptrs[i] = my_realloc(my_ptrs[i], sizeof(int) * 8);
        my_r_latencies[i] = stop_timer(&timer);
        prevent_optimization(my_ptrs[i]);
    }

    // 3. Custom Free
    for (int i = 0; i < ITERATIONS; i++) {
        start_timer(&timer);
        my_free(my_ptrs[i]);
        my_f_latencies[i] = stop_timer(&timer);
    }

    // 4. Custom Calloc
    for (int i = 0; i < ITERATIONS; i++) {
        start_timer(&timer);
        my_ptrs[i] = my_calloc(4, sizeof(int));
        my_c_latencies[i] = stop_timer(&timer);
        prevent_optimization(my_ptrs[i]);
    }
    // Cleanup custom calloc blocks cleanly
    for (int i = 0; i < ITERATIONS; i++) { my_free(my_ptrs[i]); }

    // PHASE 3: Export Expanded Data to CSV File
    FILE *csv_file = fopen("benchmark_results.csv", "w");
    if (csv_file == NULL) {
        perror("Failed to open CSV file for writing");
        exit(EXIT_FAILURE);
    }

    fprintf(csv_file, "iteration,glibc_malloc,glibc_free,glibc_calloc,glibc_realloc,custom_malloc,custom_free,custom_calloc,custom_realloc\n");

    for (int i = 0; i < ITERATIONS; i++) {
        fprintf(csv_file, "%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                i,
                glibc_m_latencies[i], glibc_f_latencies[i], glibc_c_latencies[i], glibc_r_latencies[i],
                my_m_latencies[i], my_f_latencies[i], my_c_latencies[i], my_r_latencies[i]);
    }

    fclose(csv_file);
    printf("Comprehensive benchmark finished. Data saved to 'benchmark_results.csv'.\n");

    // Clean up measurement arrays
    free(glibc_m_latencies); 
    free(glibc_f_latencies); 
    free(glibc_c_latencies); 
    free(glibc_r_latencies); 
    free(ptrs);
    free(my_m_latencies); 
    free(my_f_latencies); 
    free(my_c_latencies); 
    free(my_r_latencies); 
    free(my_ptrs);

    return 0;
}