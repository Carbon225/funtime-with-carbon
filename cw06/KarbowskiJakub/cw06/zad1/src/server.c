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
    for (int i = 0; i < SERVER_MAX_CLIENTS; ++i)
    {
        server->clients[i] = -1;
    }
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
        c2s_msg_t msg;
        ssize_t n_read = msgrcv(server->server_queue, &msg, sizeof(msg.data), 0, 0);
        if (n_read == -1)
        {
            perror("[E] Error receiving message");
            continue;
        }

        switch (msg.type)
        {
            case MESSAGE_INIT:
                printf("[I] Got INIT message\n");
                server_handle_init(server, &msg.data.init);
                break;

            default:
                printf("[E] Got unknown message: %ld\n", msg.type);
                break;
        }
    }
    printf("[I] Server loop done\n");
    return 0;
}

int server_handle_init(server_t *server, struct c2s_init_msg_t *msg)
{
    int client_id = -1;

    // find free id
    for (int i = 0; i < SERVER_MAX_CLIENTS; ++i)
    {
        if (server->clients[i] == -1)
        {
            client_id = i;
            break;
        }
    }
    if (client_id == -1)
    {
        printf("[E] Cannot connect client, client array full\n");
        return -1;
    }
    // save client queue id
    server->clients[client_id] = msg->client_queue;
    printf("[I] New client was assigned ID %d\n", client_id);

    printf("[I] Sending ID to client\n");
    s2c_msg_t resp;
    resp.type = MESSAGE_INIT;
    resp.data.init.client_id = client_id;
    int err = msgsnd(server->clients[client_id], &resp, sizeof(resp.data), 0);
    if (err)
    {
        perror("[E] Error sending ID");
        return -1;
    }
    printf("[I] OK\n");

    return 0;
}
