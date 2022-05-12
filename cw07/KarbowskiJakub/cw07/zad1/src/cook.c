#include "cook.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "pizzeria.h"
#include "furnace.h"
#include "table.h"
#include "utils.h"

int main(int argc, char **argv)
{
    srand(time(0));

    pizzeria_load();

    pizza_t pizza;

    for (;;)
    {
        pizza = cook_prepare_pizza();
        cook_put_in_furnace(pizza);
        worker_wait(4, 5);
        cook_take_out_and_send();
    }

    return 0;
}

pizza_t cook_prepare_pizza()
{
    pizza_t pizza = rand() % 10;
    printf("[cook] (%d %ld) Przygotowuje pizze: %d\n", getpid(), millis(), pizza);
    worker_wait(1, 2);
    return pizza;
}

int cook_put_in_furnace(pizza_t pizza)
{
    int usage = furnace_put(pizza);
    printf("[cook] (%d %ld) Dodalem pizze: %d. Liczba pizz w piecu: %d\n",
       getpid(),
       millis(),
       pizza,
       usage
   );

    return 0;
}

int cook_take_out_and_send()
{
    int furnace_usage;
    pizza_t pizza = furnace_get(&furnace_usage);
    int table_usage = table_put(pizza);

    printf("[cook] (%d %ld) Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n",
       getpid(),
       millis(),
       pizza,
       furnace_usage,
       table_usage
    );

    return 0;
}
