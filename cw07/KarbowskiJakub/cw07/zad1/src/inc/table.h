#ifndef JK_07_01_TABLE_H
#define JK_07_01_TABLE_H

#include "pizzeria.h"

#define TABLE_SIZE 5

int table_put(pizza_t pizza);

pizza_t table_get(int *usage);

#endif
