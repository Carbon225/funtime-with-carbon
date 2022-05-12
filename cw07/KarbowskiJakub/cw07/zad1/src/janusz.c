#include "janusz.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "pizzeria.h"
#include "table.h"
#include "utils.h"

int main(int argc, char **argv)
{
    srand(time(0));

    pizzeria_load();

    pizza_t pizza;

    for (;;)
    {
        pizza = janusz_take_from_table();
        worker_wait(4, 5);
        janusz_deliver(pizza);
        worker_wait(4, 5);
    }

    return 0;
}

pizza_t janusz_take_from_table()
{
    int usage;
    pizza_t pizza = table_get(&usage);
    printf("[janusz] (%d %ld) Pobieram pizze: %d. Liczba pizz na stole: %d\n",
       getpid(),
       millis(),
       pizza,
       usage
    );
    return pizza;
}

int janusz_deliver(pizza_t pizza)
{
    printf("[janusz] (%d %ld) Dostarczam pizze: %d\n",
       getpid(),
       millis(),
       pizza
    );
    return 0;
}
