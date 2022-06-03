#include "workshop.h"

#include <pthread.h>

static volatile bool stopped = false;
static volatile int reindeers = 0;
static volatile int reindeer_vacation_permits = 0;

static pthread_mutex_t workshop_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t workshop_cond = PTHREAD_COND_INITIALIZER;

void workshop_lock()
{
    pthread_mutex_lock(&workshop_mtx);
}

void workshop_unlock()
{
    pthread_mutex_unlock(&workshop_mtx);
}

void workshop_wait()
{
    pthread_cond_wait(&workshop_cond, &workshop_mtx);
}

void workshop_stop()
{
    if (!stopped)
    {
        stopped = true;
        pthread_cond_broadcast(&workshop_cond);
    }
}

bool workshop_is_stopped()
{
    return stopped;
}

int workshop_get_reindeers()
{
    return reindeers;
}

void workshop_reindeer_return()
{
    reindeers++;
    pthread_cond_broadcast(&workshop_cond);
}

void workshop_reindeer_leave()
{
    reindeers--;
    pthread_cond_broadcast(&workshop_cond);
}

void workshop_give_reindeer_vacation_permit()
{
    reindeer_vacation_permits++;
    pthread_cond_broadcast(&workshop_cond);
}

void workshop_get_reindeer_vacation_permit()
{
    while (!reindeer_vacation_permits)
        workshop_wait();
    reindeer_vacation_permits--;
    pthread_cond_broadcast(&workshop_cond);
}

int workshop_get_reindeer_vacation_permits()
{
    return reindeer_vacation_permits;
}
