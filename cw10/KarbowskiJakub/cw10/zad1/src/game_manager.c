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
    for (int i = 0; i < GMAN_MAX_PLAYERS; ++i)
    {
        if (!server->game_manager.players[i].active)
            continue;

        if (strcmp(name, server->game_manager.players[i].name) != 0)
            continue;

        int session = server->game_manager.players[i].session;

        if (session == -1)
            return ERR_GENERIC;

        int ret = game_move(&server->game_manager.sessions[session].game, move);

        packet_t resp;
        resp.type = PACKET_STATUS;
        resp.status.err = ret;

        if (packet_send(server->connections[server->game_manager.players[i].con].sock, &resp))
            LOGE("Failed sending response");

        if (!ret)
        {
            packet_t packet;
            packet.type = PACKET_GAME;
            packet.game.game = server->game_manager.sessions[session].game;

            packet.game.player = PLAYER_X;
            memcpy(packet.game.opponent, server->game_manager.players[server->game_manager.sessions[session].p2].name, PLAYER_NAME_MAX);
            packet_send(server->connections[server->game_manager.players[server->game_manager.sessions[session].p1].con].sock, &packet);

            packet.game.player = PLAYER_O;
            memcpy(packet.game.opponent, server->game_manager.players[server->game_manager.sessions[session].p1].name, PLAYER_NAME_MAX);
            packet_send(server->connections[server->game_manager.players[server->game_manager.sessions[session].p2].con].sock, &packet);
        }

        return ERR_OK;
    }

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

void gman_cleanup_players(server_t *server)
{
    if (!server) return;

    for (int i = 0; i < GMAN_MAX_PLAYERS; ++i)
    {
        gman_player_t *player = &server->game_manager.players[i];
        if (!player->active) continue;
        server_client_conn_t *conn = &server->connections[player->con];
        if (!conn->active)
        {
            gman_remove_player(server, i);
        }
    }
}

void gman_cleanup_sessions(server_t *server)
{
    if (!server) return;

    for (int i = 0; i < GMAN_MAX_SESSIONS; ++i)
    {
        gman_game_session_t *sess = &server->game_manager.sessions[i];
        if (!sess->active) continue;
        gman_player_t *p1 = &server->game_manager.players[sess->p1];
        gman_player_t *p2 = &server->game_manager.players[sess->p2];
        if (!p1->active || !p2->active)
        {
            gman_remove_session(server, i);
        }
    }
}

void gman_remove_player(server_t *server, int i)
{
    if (!server || i < 0 || i >= GMAN_MAX_PLAYERS) return;
    if (!server->game_manager.players[i].active) return;

    gman_player_t *player = &server->game_manager.players[i];
    server_remove_client(server, player->con);
    player->con = -1;
    player->active = false;
    player->session = -1;
    LOGI("Removed player %s", player->name);
}

void gman_remove_session(server_t *server, int i)
{
    if (!server || i < 0 || i >= GMAN_MAX_SESSIONS) return;
    if (!server->game_manager.sessions[i].active) return;

    gman_game_session_t *sess = &server->game_manager.sessions[i];

    gman_remove_player(server, sess->p1);
    gman_remove_player(server, sess->p2);

    sess->active = false;
    sess->p1 = -1;
    sess->p2 = -1;

    LOGI("Removed session %d", i);
}
