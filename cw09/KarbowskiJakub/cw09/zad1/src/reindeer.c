#include "reindeer.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "workshop.h"

void* reindeer_task(void *arg)
{
    workshop_lock();

    printf("Renifer: start\n");

    while (!workshop_is_stopped())
    {
        workshop_reindeer_return();

        printf("Renifer: czeka %d reniferow na Mikolaja\n", workshop_get_reindeers());

        if (workshop_get_reindeers() == 9)
        {
            printf("Renifer: wybudzam Mikolaja\n");
        }

        workshop_get_reindeer_vacation_permit();

        reindeer_vacation();
    }

    printf("Renifer: stop\n");

    workshop_unlock();

    return NULL;
}

void reindeer_vacation()
{
    workshop_reindeer_leave();
    workshop_unlock();
    sleep((rand() % 6) + 5);
    workshop_lock();
}
