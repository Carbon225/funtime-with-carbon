#ifndef JK_07_01_PIZZERIA_H
#define JK_07_01_PIZZERIA_H

#include "furnace.h"
#include "table.h"

enum
{
    FURNACE_LOCK_SEM,
    FURNACE_USED_SEM,
    FURNACE_FREE_SEM,
    TABLE_LOCK_SEM,
    TABLE_USED_SEM,
    TABLE_FREE_SEM,
    PIZZERIA_SEM_MAX,
};

typedef struct pizzeria_t
{
    int shm;
    int sem_set;
    furnace_t furnace;
    table_t table;
} pizzeria_t;

int pizzeria_create();

int pizzeria_delete();

int pizzeria_load();

extern pizzeria_t *pizzeria;

#endif
