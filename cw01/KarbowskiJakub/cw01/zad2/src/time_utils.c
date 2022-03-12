#include "time_utils.h"

#include <stdio.h>
#include <sys/resource.h>
#include <time.h>

static double ts_sub(const struct timespec *t1, const struct timespec *t2)
{
    return (double) (t1->tv_sec - t2->tv_sec) + (double) (t1->tv_nsec - t2->tv_nsec) / 1000000000.0;
}

static double tv_sub(const struct timeval *t1, const struct timeval *t2)
{
    return (double) (t1->tv_sec - t2->tv_sec) + (double) (t1->tv_usec - t2->tv_usec) / 1000000.0;
}

void bench_start(bench_t *bench)
{
    clock_gettime(CLOCK_MONOTONIC, &bench->ts);
    getrusage(RUSAGE_SELF, &bench->r_usage_self);
    getrusage(RUSAGE_CHILDREN, &bench->r_usage_children);
}

void bench_stop(bench_t *bench)
{
    struct timespec ts;
    struct rusage r_usage_self;
    struct rusage r_usage_children;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    getrusage(RUSAGE_SELF, &r_usage_self);
    getrusage(RUSAGE_CHILDREN, &r_usage_children);

    bench->rtime = ts_sub(&ts, &bench->ts);

    bench->utime = tv_sub(
        &r_usage_self.ru_utime,
        &bench->r_usage_self.ru_utime
    ) + tv_sub(
        &r_usage_children.ru_utime,
        &bench->r_usage_children.ru_utime
    );

    bench->stime = tv_sub(
        &r_usage_self.ru_stime,
        &bench->r_usage_self.ru_stime
    ) + tv_sub(
        &r_usage_children.ru_stime,
        &bench->r_usage_children.ru_stime
    );
}

void bench_print(const bench_t *bench)
{
    printf("real %f user %f system %f [s]\n", bench->rtime, bench->utime, bench->stime);
}
