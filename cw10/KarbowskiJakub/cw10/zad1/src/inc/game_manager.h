#ifndef JK_10_01_GAME_MANAGER_H
#define JK_10_01_GAME_MANAGER_H

#include "game.h"
#include "err.h"
#include "server.h"

void gman_init(server_t *server);

err_t gman_add_player(server_t *server, int con, const char *name);

err_t gman_execute_move(server_t *server, const char *name, pos_t move);

err_t gman_process(server_t *server);

err_t gman_find_pairs(server_t *server);

err_t gman_create_session(server_t *server, int p1, int p2);

void gman_cleanup_players(server_t *server);

void gman_cleanup_sessions(server_t *server);

void gman_remove_player(server_t *server, int i);

void gman_remove_session(server_t *server, int i);

#endif
