#ifndef JK_06_01_CLIENT_H
#define JK_06_01_CLIENT_H

#include "common.h"

typedef struct client_t
{
    int client_queue;
    int server_queue;
    int client_id;
} client_t;


int client_init(client_t *client);

void client_free(client_t *client);


int client_create_queue(client_t *client);

int client_delete_queue(client_t *client);


int client_open_server(client_t *client);

int client_loop(client_t *client);


int client_send_init(client_t *client);

int client_send_stop(client_t *client);


int client_handle_init(client_t *client, struct s2c_init_msg_t *msg);

int client_handle_stop(client_t *client);

#endif
