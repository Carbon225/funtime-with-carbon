#ifndef JK_10_01_GAME_MANAGER_H
#define JK_10_01_GAME_MANAGER_H

#include "game.h"

typedef enum gman_err_t
{
    GMAN_OK = 0,
    GMAN_ERR,
} gman_err_t;

typedef struct gman_t
{

} gman_t;

const char* gman_err_msg(gman_err_t err);

void gman_init(gman_t *man);

gman_err_t gman_add_player(gman_t *man, const char *name);

gman_err_t gman_execute_move(gman_t *man, const char *name, pos_t move);

#endif
