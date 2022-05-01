#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "common.h"

int main(int argc, char **argv)
{
    server_t server;
    if (server_init(&server)) return -1;

    int err = 0;
    do
    {
        if ((err = server_create_queue(&server))) break;

        if ((err = server_loop(&server))) break;
    } while (0);

    server_delete_queue(&server);

    server_free(&server);

    return err;
}

int server_init(server_t *server)
{
    printf("[I] Creating server\n");
    server->server_queue = -1;
    printf("[I] OK\n");
    return 0;
}

void server_free(server_t *server)
{
    printf("[I] Removing server\n");
    printf("[I] OK\n");
}


int server_create_queue(server_t *server)
{
    printf("[I] Creating server queue\n");
    const char *home = getenv("HOME");
    key_t key = ftok(home, SERVER_QUEUE_PROJ_ID);
    server->server_queue = msgget(key,IPC_CREAT | 0600);
    if (server->server_queue < 0)
    {
        perror("[E] Could not create server queue");
        return -1;
    }
    printf("[I] OK\n");
    return 0;
}

int server_delete_queue(server_t *server)
{
    printf("[I] Deleting server queue\n");
    if (msgctl(server->server_queue, IPC_RMID, NULL))
    {
        perror("[E] Could not remove server queue");
        return -1;
    }
    printf("[I] OK\n");
    return 0;
}

int server_loop(server_t *server)
{
    printf("[I] Starting server loop\n");
    for (;;)
    {
        break;
    }
    printf("[I] Server loop done\n");
    return 0;
}
