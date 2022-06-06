#ifndef JK_10_01_GAME_MANAGER_H
#define JK_10_01_GAME_MANAGER_H

#include "game.h"
#include "err.h"

typedef struct gman_t
{

} gman_t;

void gman_init(gman_t *man);

err_t gman_add_player(gman_t *man, const char *name);

err_t gman_execute_move(gman_t *man, const char *name, pos_t move);

#endif
