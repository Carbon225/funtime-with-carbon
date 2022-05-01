#ifndef JK_06_01_CLIENT_H
#define JK_06_01_CLIENT_H

#include <mqueue.h>
#include "common.h"

typedef struct client_t
{
    long client_queue_uid;
    mqd_t client_queue;
    mqd_t server_queue;
    int client_id;
} client_t;


int client_init(client_t *client);

void client_free(client_t *client);


int client_create_queue(client_t *client);

int client_delete_queue(client_t *client);


int client_open_server(client_t *client);

int client_close_server(client_t *client);

int client_loop(client_t *client);


int client_send_init(client_t *client);

int client_send_stop(client_t *client);

int client_send_list(client_t *client);

int client_send_2all(client_t *client, const char body[]);

int client_send_2one(client_t *client, int recipient_id, const char body[]);


int client_handle_init(client_t *client, struct s2c_init_msg_t *msg);

int client_handle_stop(client_t *client);

int client_handle_list(client_t *client, struct s2c_list_msg_t *msg);

int client_handle_mail(client_t *client, struct s2c_mail_msg_t *msg);

#endif
