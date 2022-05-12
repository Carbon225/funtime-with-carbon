#ifndef JK_07_01_FURNACE_H
#define JK_07_01_FURNACE_H

#include "pizza.h"

#define FURNACE_SIZE 5

int furnace_put(pizza_t pizza);

pizza_t furnace_get(int *usage);

#endif
