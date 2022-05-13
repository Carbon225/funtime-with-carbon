#include "invert_2.h"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct invert_task_arg_t
{
    int start_col;
    int block_width;
    image_t *img;
} invert_task_arg_t;

static void* invert_task(void *arg)
{
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    int start_col = ((invert_task_arg_t*) arg)->start_col;
    int block_width = ((invert_task_arg_t*) arg)->block_width;
    image_t *img = ((invert_task_arg_t*) arg)->img;
    free(arg);

    for (int x = start_col; x < img->w && x < start_col + block_width; ++x)
    {
        for (int y = 0; y < img->h; ++y)
        {
            int i = y * img->w + x;
            img->data[i] = img->max - img->data[i];
        }
    }

    struct timespec stop_time;
    clock_gettime(CLOCK_MONOTONIC, &stop_time);

    long elapsed = (stop_time.tv_sec * 1000000 + stop_time.tv_nsec / 1000) - (start_time.tv_sec * 1000000 + start_time.tv_nsec / 1000);

    return (void*) elapsed;
}

int invert_2(image_t *img, int nthr)
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
        arg->start_col = created * ((img->w + nthr - 1) / nthr);
        arg->block_width = (img->w + nthr - 1) / nthr;
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
