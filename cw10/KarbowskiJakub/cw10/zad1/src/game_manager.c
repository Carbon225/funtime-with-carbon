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
    server->game_manager.players[player_i].session = -1;

    return ERR_OK;
}

err_t gman_execute_move(server_t *server, const char *name, pos_t move)
{
    return ERR_GENERIC;
}

err_t gman_process(server_t *server)
{
    return gman_find_pairs(server);
}

err_t gman_find_pairs(server_t *server)
{
    int p1 = -1;

    for (int i = 0; i < GMAN_MAX_PLAYERS; ++i)
    {
        if (!server->game_manager.players[i].active ||
            server->game_manager.players[i].session != -1)
            continue;

        if (p1 == -1)
        {
            p1 = i;
            continue;
        }

        int p2 = i;

        if (gman_create_session(server, p1, p2))
        {
            LOGE("Could not create session");
            return ERR_GENERIC;
        }
        else
        {
            LOGI("Created session between %d and %d", p1, p2);
        }

        p1 = -1;
    }

    return ERR_OK;
}

err_t gman_create_session(server_t *server, int p1, int p2)
{
    for (int i = 0; i < GMAN_MAX_SESSIONS; ++i)
    {
        if (!server->game_manager.sessions[i].active)
        {
            server->game_manager.sessions[i].active = true;
            server->game_manager.sessions[i].p1 = p1;
            server->game_manager.sessions[i].p2 = p2;
            memset(&server->game_manager.sessions[i].game.board, 0, sizeof(board_t));
            server->game_manager.sessions[i].game.next_player = PLAYER_X;
            server->game_manager.sessions[i].game.is_over = false;

            server->game_manager.players[p1].session = i;
            server->game_manager.players[p2].session = i;

            packet_t packet;
            packet.type = PACKET_GAME;
            packet.game.game = server->game_manager.sessions[i].game;

            packet.game.player = PLAYER_X;
            memcpy(packet.game.opponent, server->game_manager.players[p2].name, PLAYER_NAME_MAX);
            packet_send(server->connections[server->game_manager.players[p1].con].sock, &packet);

            packet.game.player = PLAYER_O;
            memcpy(packet.game.opponent, server->game_manager.players[p1].name, PLAYER_NAME_MAX);
            packet_send(server->connections[server->game_manager.players[p2].con].sock, &packet);

            return ERR_OK;
        }
    }
    LOGE("Out of game sessions");
    return ERR_GENERIC;
}
