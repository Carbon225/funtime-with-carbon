#include "pizzeria.h"

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void worker_wait(int low, int high)
{
    int ms = (rand() % (1000 * (high - low))) + low * 1000;
    usleep(ms * 1000);
}

long millis()
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
}
