#include "game_manager.h"

#include <stdbool.h>
#include <string.h>

#include "log.h"

void gman_init(server_t *server)
{
    if (!server) return;

    for (int i = 0; i < GMAN_MAX_PLAYERS; ++i)
    {
        server->game_manager.players[i].active = false;
    }

    for (int i = 0; i < GMAN_MAX_SESSIONS; ++i)
    {
        server->game_manager.sessions[i].active = false;
    }
}

err_t gman_add_player(server_t *server, int con, const char *name)
{
    if (!server) return ERR_GENERIC;

    int player_i = -1;
    bool duplicate = false;

    for (int i = 0; i < GMAN_MAX_PLAYERS; ++i)
    {
        if (!server->game_manager.players[i].active)
            player_i = i;
        else if (!strcmp(name, server->game_manager.players[i].name))
        {
            duplicate = true;
            break;
        }
    }

    if (duplicate)
    {
        LOGE("%s", err_msg(ERR_NAME_OCCUPIED));
        return ERR_NAME_OCCUPIED;
    }

    if (player_i == -1)
    {
        LOGE("Out of gman player space");
        return ERR_GENERIC;
    }

    server->game_manager.players[player_i].active = true;
    server->game_manager.players[player_i].con = con;
    memcpy(server->game_manager.players[player_i].name, name, PLAYER_NAME_MAX);

    return ERR_OK;
}

err_t gman_execute_move(server_t *server, const char *name, pos_t move)
{
    return ERR_GENERIC;
}

err_t gman_process(server_t *server)
{
    return ERR_OK;
}
