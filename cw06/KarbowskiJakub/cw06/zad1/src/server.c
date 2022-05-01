#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include "common.h"

static volatile bool g_should_stop = false;

static void sig_handler(int sig)
{
    switch (sig)
    {
        case SIGINT:
            g_should_stop = true;
            break;

        default:
            break;
    }
}

int main(int argc, char **argv)
{
    printf("[I] Installing SIGINT handler\n");
    struct sigaction act = {0};
    act.sa_handler = sig_handler;
    if (sigaction(SIGINT, &act, NULL))
    {
        perror("[E] Could not install handler");
        return -1;
    }
    printf("[I] OK\n");

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
    server->n_clients = 0;
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
    if (server->server_queue != -1)
    {
        printf("[E] Cannot create server queue, already exists\n");
        return -1;
    }

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
    if (server->server_queue == -1)
    {
        printf("[E] Cannot delete server queue, not created\n");
        return -1;
    }

    printf("[I] Deleting server queue\n");
    if (msgctl(server->server_queue, IPC_RMID, NULL))
    {
        perror("[E] Could not remove server queue");
        return -1;
    }
    server->server_queue = -1;
    printf("[I] OK\n");
    return 0;
}

int server_loop(server_t *server)
{
    if (server->server_queue == -1)
    {
        printf("[E] Cannot start loop, server queue not opened\n");
        return -1;
    }

    bool is_stopping = false;

    printf("[I] Starting server loop\n");
    while (!g_should_stop || server->n_clients > 0)
    {
        if (g_should_stop && !is_stopping)
        {
            server_stop(server);
            is_stopping = true;
        }

        c2s_msg_t msg;
        ssize_t n_read = msgrcv(server->server_queue, &msg, sizeof(msg.data), -MESSAGE_MAX, 0);
        if (n_read == -1)
        {
            if (errno == EINTR)
                printf("[I] Interrupted by signal\n");
            else
                perror("[E] Error receiving message");
            continue;
        }

        switch (msg.type)
        {
            case MESSAGE_INIT:
                printf("[I] Got INIT message\n");
                server_handle_init(server, &msg.data.init);
                break;

            case MESSAGE_STOP:
                printf("[I] Got STOP message\n");
                server_handle_stop(server, &msg.data.stop);
                break;

            case MESSAGE_LIST:
                printf("[I] Got LIST message\n");
                server_handle_list(server, &msg.data.list);
                break;

            case MESSAGE_2ALL:
                printf("[I] Got 2ALL message\n");
                server_handle_2all(server, &msg.data.to_all);
                break;

            case MESSAGE_2ONE:
                printf("[I] Got 2ONE message\n");
                server_handle_2one(server, &msg.data.to_one);
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
    server->n_clients++;
    printf("[I] New client was assigned ID %d\n", client_id);

    printf("[I] Sending ID to client\n");
    if (server_send_init(server, client_id))
        return -1;
    printf("[I] OK\n");

    return 0;
}

int server_send_init(server_t *server, int client_id)
{
    if (server->clients[client_id] == -1)
    {
        printf("[E] Cannot send INIT to %d, client not registered\n", client_id);
        return -1;
    }

    s2c_msg_t resp;
    resp.type = MESSAGE_INIT;
    resp.data.init.client_id = client_id;
    int err = msgsnd(server->clients[client_id], &resp, sizeof(resp.data), 0);
    if (err)
    {
        perror("[E] Error sending ID");
        return -1;
    }
    return 0;
}

int server_stop(server_t *server)
{
    printf("[I] Sending STOP to clients\n");

    for (int i = 0; i < SERVER_MAX_CLIENTS; ++i)
    {
        if (server->clients[i] != -1)
        {
            server_send_stop(server, i);
        }
    }

    printf("[I] OK\n");
    return 0;
}

int server_send_stop(server_t *server, int client_id)
{
    if (server->clients[client_id] == -1)
    {
        printf("[E] Cannot send STOP to %d, client not registered\n", client_id);
        return -1;
    }

    printf("[I] Sending STOP to %d\n", client_id);

    s2c_msg_t msg;
    msg.type = MESSAGE_STOP;
    int err = msgsnd(server->clients[client_id], &msg, sizeof(msg.data), 0);
    if (err)
    {
        perror("[E] Error sending STOP");
        return -1;
    }

    printf("[I] OK\n");
    return 0;
}

int server_handle_stop(server_t *server, struct c2s_stop_msg_t *msg)
{
    if (server->clients[msg->client_id] == -1)
    {
        printf("[E] Cannot remove client %d, not registered\n", msg->client_id);
        return -1;
    }

    server->clients[msg->client_id] = -1;
    server->n_clients--;
    printf("[I] Removed client %d\n", msg->client_id);
    return 0;
}

int server_handle_list(server_t *server, struct c2s_list_msg_t *msg)
{
    printf("[I] Sending list of clients to %d\n", msg->client_id);
    for (int i = 0; i < SERVER_MAX_CLIENTS; ++i)
    {
        if (server->clients[i] != -1)
        {
            server_send_list(server, msg->client_id, i);
        }
    }
    printf("[I] OK\n");
    return 0;
}

int server_handle_2all(server_t *server, struct c2s_2all_msg_t *msg)
{
    printf("[I] Sending mail to all clients from %d\n", msg->client_id);
    for (int i = 0; i < SERVER_MAX_CLIENTS; ++i)
    {
        if (server->clients[i] != -1)
        {
            server_send_mail(server, msg->client_id, i, msg->body);
        }
    }
    printf("[I] OK\n");
    return 0;
}

int server_handle_2one(server_t *server, struct c2s_2one_msg_t *msg)
{
    return server_send_mail(server, msg->client_id, msg->recipient_id, msg->body);
}

int server_send_list(server_t *server, int target_client_id, int active_client)
{
    printf("[I] Sending client %d to %d\n", active_client, target_client_id);

    s2c_msg_t msg;
    msg.type = MESSAGE_LIST;
    msg.data.list.client_id = active_client;
    int err = msgsnd(server->clients[target_client_id], &msg, sizeof(msg.data), 0);
    if (err)
    {
        perror("[E] Error sending LIST");
        return -1;
    }

    printf("[I] OK\n");
    return 0;
}

int server_send_mail(server_t *server, int sender_id, int recipient_id, const char body[])
{
    printf("[I] Sending mail from %d to %d\n", sender_id, recipient_id);

    s2c_msg_t msg;
    msg.type = MESSAGE_2ONE;
    msg.data.mail.sender_id = sender_id;
    strncpy(msg.data.mail.body, body, MESSAGE_MAX_BODY_SIZE);
    msg.data.mail.body[MESSAGE_MAX_BODY_SIZE] = 0;
    int err = msgsnd(server->clients[recipient_id], &msg, sizeof(msg.data), 0);
    if (err)
    {
        perror("[E] Error sending mail");
        return -1;
    }

    printf("[I] OK\n");
    return 0;
}
