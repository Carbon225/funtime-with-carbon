#ifndef JK_06_01_SERVER_H
#define JK_06_01_SERVER_H

#include <stdbool.h>
#include "common.h"

#define SERVER_MAX_CLIENTS (256)

typedef struct server_t
{
    int server_queue;
    // client queue ids
    int clients[SERVER_MAX_CLIENTS];
    int n_clients;
} server_t;


int server_init(server_t *server);

void server_free(server_t *server);


int server_create_queue(server_t *server);

int server_delete_queue(server_t *server);


int server_loop(server_t *server);

int server_stop(server_t *server);


int server_send_init(server_t *server, int client_id);

int server_send_stop(server_t *server, int client_id);


int server_handle_init(server_t *server, struct c2s_init_msg_t *msg);

int server_handle_stop(server_t *server, struct c2s_stop_msg_t *msg);

#endif
