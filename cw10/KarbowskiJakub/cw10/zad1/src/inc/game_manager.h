#ifndef JK_10_01_GAME_MANAGER_H
#define JK_10_01_GAME_MANAGER_H

#include "game.h"
#include "err.h"
#include "server.h"

void gman_init(server_t *server);

err_t gman_add_player(server_t *server, int con, const char *name);

err_t gman_execute_move(server_t *server, const char *name, pos_t move);

err_t gman_process(server_t *server);

#endif
