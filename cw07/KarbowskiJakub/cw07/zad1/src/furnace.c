#include "furnace.h"

#include <sys/sem.h>

#include "pizzeria.h"

int furnace_put(pizza_t pizza)
{
    struct sembuf ops[3];

    ops[0].sem_num = FURNACE_LOCK_SEM;
    ops[0].sem_op = -1;
    ops[0].sem_flg = 0;

    ops[1].sem_num = FURNACE_USED_SEM;
    ops[1].sem_op = 1;
    ops[1].sem_flg = 0;

    ops[2].sem_num = FURNACE_FREE_SEM;
    ops[2].sem_op = -1;
    ops[2].sem_flg = 0;

    semop(pizzeria->sem_set, ops, 3);

    pizzeria->furnace.arr[pizzeria->furnace.write_head] = pizza;
    pizzeria->furnace.write_head = (pizzeria->furnace.write_head + 1) % FURNACE_SIZE;

    pizzeria->furnace.usage++;
    int usage = pizzeria->furnace.usage;

    ops[0].sem_op = 1;
    semop(pizzeria->sem_set, ops, 1);

    return usage;
}

pizza_t furnace_get(int *usage)
{
    struct sembuf ops[3];

    ops[0].sem_num = FURNACE_LOCK_SEM;
    ops[0].sem_op = -1;
    ops[0].sem_flg = 0;

    ops[1].sem_num = FURNACE_USED_SEM;
    ops[1].sem_op = -1;
    ops[1].sem_flg = 0;

    ops[2].sem_num = FURNACE_FREE_SEM;
    ops[2].sem_op = 1;
    ops[2].sem_flg = 0;

    semop(pizzeria->sem_set, ops, 3);

    pizza_t pizza = pizzeria->furnace.arr[pizzeria->furnace.read_head];
    pizzeria->furnace.read_head = (pizzeria->furnace.read_head + 1) % FURNACE_SIZE;

    pizzeria->furnace.usage--;

    if (usage)
        *usage = pizzeria->furnace.usage;

    ops[0].sem_op = 1;
    semop(pizzeria->sem_set, ops, 1);

    return pizza;
}
