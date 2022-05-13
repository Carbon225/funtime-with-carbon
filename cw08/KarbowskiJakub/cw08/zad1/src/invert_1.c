#include "invert_1.h"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct invert_task_arg_t
{
    int start;
    int step;
    image_t *img;
} invert_task_arg_t;

static void* invert_task(void *arg)
{
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    int start = ((invert_task_arg_t*) arg)->start;
    int step = ((invert_task_arg_t*) arg)->step;
    image_t *img = ((invert_task_arg_t*) arg)->img;
    free(arg);

    for (int i = start; i < img->h * img->w; i += step)
    {
        img->data[i] = img->max - img->data[i];
    }

    struct timespec stop_time;
    clock_gettime(CLOCK_MONOTONIC, &stop_time);

    long elapsed = (stop_time.tv_sec * 1000000 + stop_time.tv_nsec / 1000) - (start_time.tv_sec * 1000000 + start_time.tv_nsec / 1000);

    return (void*) elapsed;
}

int invert_1(image_t *img, int nthr)
{
    if (!img) return -1;

    long *times = malloc(nthr * sizeof *times);
    if (!times) return -1;

    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    pthread_t *threads = malloc(nthr * sizeof *threads);
    if (!threads)
    {
        free(times);
        return -1;
    }

    int created = 0;

    for (; created < nthr; ++created)
    {
        invert_task_arg_t *arg = malloc(sizeof *arg);
        if (!arg) break;
        arg->start = created;
        arg->step = nthr;
        arg->img = img;
        pthread_create(&threads[created], NULL, invert_task, arg);
    }

    for (int i = 0; i < created; ++i)
    {
        void *ret;
        pthread_join(threads[i], &ret);
        times[i] = (long) ret;
    }

    free(threads);

    struct timespec stop_time;
    clock_gettime(CLOCK_MONOTONIC, &stop_time);
    long elapsed = (stop_time.tv_sec * 1000000 + stop_time.tv_nsec / 1000) - (start_time.tv_sec * 1000000 + start_time.tv_nsec / 1000);

    for (int i = 0; i < created; ++i)
    {
        printf("[%d]: %ld us\n", i, times[i]);
    }

    printf("[main]: %ld us\n", elapsed);

    free(times);

    return 0;
}
