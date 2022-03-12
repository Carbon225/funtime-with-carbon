#ifndef JK_ZAD2_SRC_INC_TIME_UTILS_H
#define JK_ZAD2_SRC_INC_TIME_UTILS_H

#include <sys/resource.h>
#include <time.h>

typedef struct bench_t
{
    struct timespec ts;
    struct rusage r_usage_self;
    struct rusage r_usage_children;

    double rtime;
    double utime;
    double stime;
} bench_t;

void bench_start(bench_t *bench);

void bench_stop(bench_t *bench);

void bench_print(const bench_t *bench);

#endif
