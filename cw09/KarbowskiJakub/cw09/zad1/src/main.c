#include <stdio.h>
#include <pthread.h>

#include "workshop.h"
#include "santa.h"
#include "reindeer.h"
#include "elf.h"

int main(int argc, char **argv)
{
    pthread_t santa;
    pthread_t elfs[N_ELF];
    pthread_t reindeers[N_REINDEER];

    pthread_create(&santa, NULL, santa_task, NULL);
    for (int i = 0; i < N_ELF; ++i)
    {
        pthread_create(&elfs[i], NULL, elf_task, NULL);
    }
    for (int i = 0; i < N_REINDEER; ++i)
    {
        pthread_create(&reindeers[i], NULL, reindeer_task, NULL);
    }

    pthread_join(santa, NULL);
    for (int i = 0; i < N_ELF; ++i)
    {
        pthread_join(elfs[i], NULL);
    }
    for (int i = 0; i < N_REINDEER; ++i)
    {
        pthread_join(reindeers[i], NULL);
    }

    return 0;
}
