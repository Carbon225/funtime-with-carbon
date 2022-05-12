#ifndef JK_07_01_PIZZERIA_H
#define JK_07_01_PIZZERIA_H

#include <semaphore.h>

#include "furnace.h"
#include "table.h"

typedef struct pizzeria_t
{
    furnace_t furnace;
    table_t table;
} pizzeria_t;

typedef struct pizzeria_local_t
{
    struct
    {
        sem_t *lock_sem;
        sem_t *used_sem;
        sem_t *free_sem;
    } furnace;

    struct
    {
        sem_t *lock_sem;
        sem_t *used_sem;
        sem_t *free_sem;
    } table;
} pizzeria_local_t;

int pizzeria_create();

int pizzeria_delete();

int pizzeria_load();

extern pizzeria_t *pizzeria;

extern pizzeria_local_t pizzeria_local;

#endif
