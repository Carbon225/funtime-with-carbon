#ifndef JK_07_01_TABLE_H
#define JK_07_01_TABLE_H

#include "pizza.h"

#define TABLE_SIZE 5

typedef struct table_t
{
    pizza_t arr[TABLE_SIZE];
    int read_head;
    int write_head;
    int usage;
} table_t;

int table_put(pizza_t pizza);

pizza_t table_get(int *usage);

#endif
