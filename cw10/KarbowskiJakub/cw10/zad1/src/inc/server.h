#ifndef JK_10_01_SERVER_H
#define JK_10_01_SERVER_H

#include <stdint.h>
#include <stdbool.h>

#include "packet.h"
#include "game_manager.h"

#define SERVER_MAX_CONNECTIONS (256)

typedef enum connection_status_t
{
    CONNECTION_STATUS_DISCONNECTED,
    CONNECTION_STATUS_ACTIVE,
} connection_status_t;

typedef struct server_client_conn_t
{
    connection_status_t status;
    int sock;
    uint8_t recv_buf[PACKET_MAX_SIZE];
    int recv_count;
} server_client_conn_t;

typedef struct server_t
{
    int netsock;
    server_client_conn_t connections[SERVER_MAX_CONNECTIONS];
    gman_t game_manager;
} server_t;

err_t server_open(server_t *server, short port);

void server_close(server_t *server);

err_t server_loop(server_t *server);

err_t server_open_net_sock(server_t *server, short port);

err_t server_handle_packet(server_t *server, int con, const packet_t *packet);

err_t server_add_connection(server_t *server, int sock);

err_t server_handle_init(server_t *server, int con, const init_packet_t *packet);

err_t server_handle_move(server_t *server, int con, const move_packet_t *packet);

#endif
