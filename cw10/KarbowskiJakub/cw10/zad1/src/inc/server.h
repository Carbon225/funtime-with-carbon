#ifndef JK_10_01_SERVER_H
#define JK_10_01_SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "game.h"
#include "packet.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define GMAN_MAX_SESSIONS (256)
#define GMAN_MAX_PLAYERS (256)
#define SERVER_MAX_CONNECTIONS MIN(256, FOPEN_MAX - 1)

typedef struct server_client_conn_t
{
    bool active;
    int sock;
    uint8_t recv_buf[PACKET_MAX_SIZE];
    int recv_count;
    bool error;
    bool alive;
    pthread_mutex_t mtx;
} server_client_conn_t;

typedef struct gman_player_t
{
    bool active;
    int con;
    char name[PLAYER_NAME_MAX + 1];
    int session;
} gman_player_t;

typedef struct gman_game_session_t
{
    bool active;
    int p1;
    int p2;
    game_t game;
} gman_game_session_t;

typedef struct gman_t
{
    gman_player_t players[GMAN_MAX_PLAYERS];
    gman_game_session_t sessions[GMAN_MAX_SESSIONS];
} gman_t;

typedef struct server_t
{
    int netsock;
    int unixsock;
    char unsockpath[100];
    server_client_conn_t connections[SERVER_MAX_CONNECTIONS];
    gman_t game_manager;
    pthread_t pinger;
} server_t;

err_t server_open(server_t *server, short port, const char *sock_path);

void server_close(server_t *server);

err_t server_loop(server_t *server);

err_t server_open_net_sock(server_t *server, short port);

err_t server_open_unix_sock(server_t *server, const char *sock_path);

err_t server_handle_packet(server_t *server, int con, const packet_t *packet);

err_t server_add_connection(server_t *server, int sock);

err_t server_handle_init(server_t *server, int con, const init_packet_t *packet);

err_t server_handle_move(server_t *server, int con, const move_packet_t *packet);

void server_cleanup_clients(server_t *server);

void server_remove_client(server_t *server, int i);

#endif
