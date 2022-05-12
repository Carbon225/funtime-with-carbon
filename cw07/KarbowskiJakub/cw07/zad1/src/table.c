#include "table.h"

#include <sys/sem.h>

#include "pizzeria.h"

int table_put(pizza_t pizza)
{
    struct sembuf ops[3];

    ops[0].sem_num = TABLE_LOCK_SEM;
    ops[0].sem_op = -1;
    ops[0].sem_flg = 0;

    ops[1].sem_num = TABLE_USED_SEM;
    ops[1].sem_op = 1;
    ops[1].sem_flg = 0;

    ops[2].sem_num = TABLE_FREE_SEM;
    ops[2].sem_op = -1;
    ops[2].sem_flg = 0;

    semop(pizzeria->sem_set, ops, 3);

    pizzeria->table.arr[pizzeria->table.write_head] = pizza;
    pizzeria->table.write_head = (pizzeria->table.write_head + 1) % TABLE_SIZE;

    pizzeria->table.usage++;
    int usage = pizzeria->table.usage;

    ops[0].sem_op = 1;
    semop(pizzeria->sem_set, ops, 1);

    return usage;
}

pizza_t table_get(int *usage)
{
    struct sembuf ops[3];

    ops[0].sem_num = TABLE_LOCK_SEM;
    ops[0].sem_op = -1;
    ops[0].sem_flg = 0;

    ops[1].sem_num = TABLE_USED_SEM;
    ops[1].sem_op = -1;
    ops[1].sem_flg = 0;

    ops[2].sem_num = TABLE_FREE_SEM;
    ops[2].sem_op = 1;
    ops[2].sem_flg = 0;

    semop(pizzeria->sem_set, ops, 3);

    pizza_t pizza = pizzeria->table.arr[pizzeria->table.read_head];
    pizzeria->table.read_head = (pizzeria->table.read_head + 1) % TABLE_SIZE;

    pizzeria->table.usage--;

    if (usage)
        *usage = pizzeria->table.usage;

    ops[0].sem_op = 1;
    semop(pizzeria->sem_set, ops, 1);

    return pizza;
}
