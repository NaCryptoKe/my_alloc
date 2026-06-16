#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <time.h>

typedef struct {
    struct timespec start;
    struct timespec end;
} bench_timer_t;

static inline void start_timer(bench_timer_t *t) {
    clock_gettime(CLOCK_MONOTONIC, &t->start);
}

static inline double stop_timer(bench_timer_t *t) {
    clock_gettime(CLOCK_MONOTONIC, &t->end);

    double start_ns = (t->start.tv_sec * 1e9) + t->start.tv_nsec;
    double end_ns = (t->end.tv_sec * 1e9) + t->end.tv_nsec;
    return end_ns - start_ns;
}

static inline void prevent_optimization (void *ptr) {
    asm volatile ("" : : "g"(ptr) : "memory");
}

#endif //BENCHMARK_H