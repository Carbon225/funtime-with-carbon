#include "santa.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "workshop.h"

void* santa_task(void *arg)
{
    workshop_lock();

    printf("Mikolaj: start\n");

    for (;;)
    {
        santa_sleep();

        if (workshop_is_stopped())
        {
            break;
        }

        printf("Mikolaj: budze sie\n");

        if (workshop_get_reindeers() == 9)
        {
            santa_deliver_presents();
            for (int i = 0; i < N_REINDEER; ++i)
            {
                workshop_give_reindeer_vacation_permit();
            }
            while (workshop_get_reindeer_vacation_permits() > 0)
            {
                workshop_wait();
            }
        }

        if (workshop_get_elfs() == 3)
        {
            printf("Mikolaj: rozwiazuje problemy elfow %d %d %d\n", workshop_get_elf(0), workshop_get_elf(1), workshop_get_elf(2));
            workshop_unlock();
            sleep((rand() % 2) + 1);
            workshop_lock();
            for (int i = 0; i < 3; ++i)
            {
                workshop_elf_give_help();
            }
        }

        printf("Mikolaj: zasypiam\n");
    }

    printf("Mikolaj: stop\n");

    workshop_unlock();

    return NULL;
}

void santa_sleep()
{
    while (!(workshop_get_reindeers() == 9 ||
             workshop_get_elfs() == 3 ||
             workshop_is_stopped()))
        workshop_wait();
}

void santa_deliver_presents()
{
    static int delivered_presents = 0;

    printf("Mikolaj: dostarczam zabawki\n");
    workshop_unlock();
    sleep((rand() % 3) + 2);
    workshop_lock();

    delivered_presents++;
    if (delivered_presents >= 3)
    {
        workshop_stop();
    }
}
