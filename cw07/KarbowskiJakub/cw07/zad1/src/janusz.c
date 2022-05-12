#include "janusz.h"

#include <stdio.h>
#include <unistd.h>

#include "table.h"
#include "utils.h"

int main(int argc, char **argv)
{
    printf("janusz\n");

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
    printf("(%d %ld) Pobieram pizze: %d. Liczba pizz na stole: %d\n",
       getpid(),
       millis(),
       pizza,
       usage
    );
    return pizza;
}

int janusz_deliver(pizza_t pizza)
{
    printf("(%d %ld) Dostarczam pizze: %d\n",
       getpid(),
       millis(),
       pizza
    );
    return 0;
}
