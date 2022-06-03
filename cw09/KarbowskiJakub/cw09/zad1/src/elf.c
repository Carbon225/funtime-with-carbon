#include "elf.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "workshop.h"

void* elf_task(void *arg)
{
    int id = (int)(ssize_t) arg;

    workshop_lock();

    printf("Elf: start, %d\n", id);

    while (!workshop_is_stopped())
    {
        workshop_unlock();
        sleep((rand() % 4) + 2);
        workshop_lock();

        if (workshop_get_elfs() >= 3)
        {
            printf("Elf: czeka na powrot elfow, %d\n", id);
        }

        while (!(workshop_get_elfs() < 3 || workshop_is_stopped()))
            workshop_wait();

        if (workshop_is_stopped())
            break;

        printf("Elf: czeka %d elfow na Mikolaja, %d\n", workshop_get_elfs() + 1, id);

        workshop_elf_queue(id);

        if (workshop_get_elfs() == 3)
        {
            printf("Elf: wybudzam Mikolaja, %d\n", id);
        }

        workshop_elf_get_help();

        printf("Elf: Mikolaj rozwiazuje problem, %d\n", id);
    }

    printf("Elf: stop, %d\n", id);

    workshop_unlock();

    return NULL;
}
