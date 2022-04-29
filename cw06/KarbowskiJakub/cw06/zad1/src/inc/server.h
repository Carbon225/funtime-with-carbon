#ifndef JK_06_01_SERVER_H
#define JK_06_01_SERVER_H

#include <stdbool.h>

typedef struct server_t
{
    int server_queue;
} server_t;


int server_init(server_t *server);

void server_free(server_t *server);


int server_create_queue(server_t *server);

int server_delete_queue(server_t *server);


int server_loop(server_t *server);

#endif
