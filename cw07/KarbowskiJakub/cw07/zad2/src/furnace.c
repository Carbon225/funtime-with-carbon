#include "furnace.h"

#include "pizzeria.h"

int furnace_put(pizza_t pizza)
{
    // reserve free space
    sem_wait(pizzeria_local.furnace.free_sem);

    // lock
    sem_wait(pizzeria_local.furnace.lock_sem);

    // Above operations are not atomic but:
    // 1. a free slot is guaranteed to be reserved
    // 2. exclusive access is acquired
    // Therefore, modification is safe.

    // after locking, insert new element
    // assuming free space is available (it has been reserved)
    pizzeria->furnace.arr[pizzeria->furnace.write_head] = pizza;
    pizzeria->furnace.write_head = (pizzeria->furnace.write_head + 1) % FURNACE_SIZE;

    pizzeria->furnace.usage++;
    int usage = pizzeria->furnace.usage;

    // inform processes that a new element can be read
    sem_post(pizzeria_local.furnace.used_sem);
    // will not wake other processes because array is locked

    // unlock
    sem_post(pizzeria_local.furnace.lock_sem);
    // now other processes can read array

    return usage;
}

pizza_t furnace_get(int *usage)
{
    // reserve one element for reading
    sem_wait(pizzeria_local.furnace.used_sem);

    // lock
    sem_wait(pizzeria_local.furnace.lock_sem);

    // One element has been reserved for reading.
    // Exclusive access is acquired.

    // read oldest element
    pizza_t pizza = pizzeria->furnace.arr[pizzeria->furnace.read_head];
    pizzeria->furnace.read_head = (pizzeria->furnace.read_head + 1) % FURNACE_SIZE;

    pizzeria->furnace.usage--;

    if (usage)
        *usage = pizzeria->furnace.usage;

    // inform processes that new space is available
    sem_post(pizzeria_local.furnace.free_sem);

    // unlock
    sem_post(pizzeria_local.furnace.lock_sem);
    // processes waiting on free_sem can resume

    return pizza;
}
