#include "pizzeria.h"

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define PIZZERIA_SHM_PATH "/jk_07_02_pizzeria_shm"
#define PIZZERIA_SEM_PATH_BASE "/jk_07_02_pizzeria_sem_"

pizzeria_t *pizzeria;
pizzeria_local_t pizzeria_local;

int pizzeria_create()
{
    int mem = shm_open(PIZZERIA_SHM_PATH, O_RDWR | O_CREAT, 0600);
    ftruncate(mem, sizeof *pizzeria);
    pizzeria = mmap(0, sizeof *pizzeria, PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);

    pizzeria->furnace.write_head = 0;
    pizzeria->furnace.read_head = 0;
    pizzeria->furnace.usage = 0;

    pizzeria->table.write_head = 0;
    pizzeria->table.read_head = 0;
    pizzeria->table.usage = 0;

    pizzeria_local.furnace.free_sem = sem_open(PIZZERIA_SEM_PATH_BASE"furnace_free", O_RDWR | O_CREAT, 0600, FURNACE_SIZE);
    pizzeria_local.furnace.used_sem = sem_open(PIZZERIA_SEM_PATH_BASE"furnace_used", O_RDWR | O_CREAT, 0600, 0);
    pizzeria_local.furnace.lock_sem = sem_open(PIZZERIA_SEM_PATH_BASE"furnace_lock", O_RDWR | O_CREAT, 0600, 1);

    pizzeria_local.table.free_sem = sem_open(PIZZERIA_SEM_PATH_BASE"table_free", O_RDWR | O_CREAT, 0600, TABLE_SIZE);
    pizzeria_local.table.used_sem = sem_open(PIZZERIA_SEM_PATH_BASE"table_used", O_RDWR | O_CREAT, 0600, 0);
    pizzeria_local.table.lock_sem = sem_open(PIZZERIA_SEM_PATH_BASE"table_lock", O_RDWR | O_CREAT, 0600, 1);

    return 0;
}

int pizzeria_delete()
{
    shm_unlink(PIZZERIA_SHM_PATH);

    sem_unlink(PIZZERIA_SEM_PATH_BASE"furnace_free");
    sem_unlink(PIZZERIA_SEM_PATH_BASE"furnace_used");
    sem_unlink(PIZZERIA_SEM_PATH_BASE"furnace_lock");

    sem_unlink(PIZZERIA_SEM_PATH_BASE"table_free");
    sem_unlink(PIZZERIA_SEM_PATH_BASE"table_used");
    sem_unlink(PIZZERIA_SEM_PATH_BASE"table_lock");

    return 0;
}

int pizzeria_load()
{
    int mem = shm_open(PIZZERIA_SHM_PATH, O_RDWR, 0600);
    pizzeria = mmap(0, sizeof *pizzeria, PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);

    pizzeria_local.furnace.free_sem = sem_open(PIZZERIA_SEM_PATH_BASE"furnace_free", O_RDWR);
    pizzeria_local.furnace.used_sem = sem_open(PIZZERIA_SEM_PATH_BASE"furnace_used", O_RDWR);
    pizzeria_local.furnace.lock_sem = sem_open(PIZZERIA_SEM_PATH_BASE"furnace_lock", O_RDWR);

    pizzeria_local.table.free_sem = sem_open(PIZZERIA_SEM_PATH_BASE"table_free", O_RDWR);
    pizzeria_local.table.used_sem = sem_open(PIZZERIA_SEM_PATH_BASE"table_used", O_RDWR);
    pizzeria_local.table.lock_sem = sem_open(PIZZERIA_SEM_PATH_BASE"table_lock", O_RDWR);

    return 0;
}
