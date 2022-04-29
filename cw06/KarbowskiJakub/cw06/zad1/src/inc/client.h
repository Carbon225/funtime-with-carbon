#ifndef JK_06_01_CLIENT_H
#define JK_06_01_CLIENT_H

typedef struct client_t
{
    int client_queue;
    int server_queue;
} client_t;


int client_init(client_t *client);

void client_free(client_t *client);


int client_create_queue(client_t *client);

int client_delete_queue(client_t *client);


int client_open_server(client_t *client);

int client_send_init(client_t *client);


int client_loop(client_t *client);

#endif
