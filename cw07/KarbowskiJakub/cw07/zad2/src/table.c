#include "table.h"

#include "pizzeria.h"

int table_put(pizza_t pizza)
{
    // reserve free space
    sem_wait(pizzeria_local.table.free_sem);

    // lock
    sem_wait(pizzeria_local.table.lock_sem);

    // Above operations are not atomic but:
    // 1. a free slot is guaranteed to be reserved
    // 2. exclusive access is acquired
    // Therefore, modification is safe.

    // after locking, insert new element
    // assuming free space is available (it has been reserved)
    pizzeria->table.arr[pizzeria->table.write_head] = pizza;
    pizzeria->table.write_head = (pizzeria->table.write_head + 1) % TABLE_SIZE;

    pizzeria->table.usage++;
    int usage = pizzeria->table.usage;

    // inform processes that a new element can be read
    sem_post(pizzeria_local.table.used_sem);
    // will not wake other processes because array is locked

    // unlock
    sem_post(pizzeria_local.table.lock_sem);
    // now other processes can read array

    return usage;
}

pizza_t table_get(int *usage)
{
    // reserve one element for reading
    sem_wait(pizzeria_local.table.used_sem);

    // lock
    sem_wait(pizzeria_local.table.lock_sem);

    // One element has been reserved for reading.
    // Exclusive access is acquired.

    // read oldest element
    pizza_t pizza = pizzeria->table.arr[pizzeria->table.read_head];
    pizzeria->table.read_head = (pizzeria->table.read_head + 1) % TABLE_SIZE;

    pizzeria->table.usage--;

    if (usage)
        *usage = pizzeria->table.usage;

    // inform processes that new space is available
    sem_post(pizzeria_local.table.free_sem);

    // unlock
    sem_post(pizzeria_local.table.lock_sem);
    // processes waiting on free_sem can resume

    return pizza;
}
