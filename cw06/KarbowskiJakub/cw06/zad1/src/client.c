#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

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

    client_t client;
    if (client_init(&client)) return -1;

    int err = 0;
    do
    {
        if ((err = client_create_queue(&client))) break;

        if ((err = client_open_server(&client))) break;

        if ((err = client_send_init(&client))) break;

        if ((err = client_loop(&client))) break;
    } while (0);

    client_send_stop(&client);

    client_delete_queue(&client);

    client_free(&client);

    return err;
}

int client_init(client_t *client)
{
    printf("[I] Creating client\n");
    client->client_queue = -1;
    client->server_queue = -1;
    client->client_id = -1;
    printf("[I] OK\n");
    return 0;
}

void client_free(client_t *client)
{
    printf("[I] Removing client\n");
    printf("[I] OK\n");
}


int client_create_queue(client_t *client)
{
    if (client->client_queue != -1)
    {
        printf("[E] Cannot create client queue, already exists\n");
        return -1;
    }

    printf("[I] Creating client queue\n");
    client->client_queue = msgget(IPC_PRIVATE, 0600);
    if (client->client_queue == -1)
    {
        perror("[E] Could not create client queue");
        return -1;
    }
    printf("[I] OK\n");
    return 0;
}

int client_delete_queue(client_t *client)
{
    if (client->client_queue == -1)
    {
        printf("[E] Cannot delete client queue, not created\n");
        return -1;
    }

    printf("[I] Deleting client queue\n");
    if (msgctl(client->client_queue, IPC_RMID, NULL))
    {
        perror("[E] Count not remove client queue");
        return -1;
    }
    client->client_queue = -1;
    printf("[I] OK\n");
    return 0;
}


int client_open_server(client_t *client)
{
    if (client->server_queue != -1)
    {
        printf("[E] Cannot open server queue, already open\n");
        return -1;
    }

    printf("[I] Opening server queue\n");
    const char *home = getenv("HOME");
    key_t key = ftok(home, SERVER_QUEUE_PROJ_ID);
    client->server_queue = msgget(key,0600);
    if (client->server_queue == -1)
    {
        perror("[E] Could not open server queue");
        return -1;
    }
    printf("[I] OK\n");
    return 0;
}

int client_send_init(client_t *client)
{
    if (client->client_queue == -1)
    {
        printf("[E] Cannot send INIT, client queue not opened\n");
        return -1;
    }

    if (client->server_queue == -1)
    {
        printf("[E] Cannot send INIT, server queue not opened\n");
        return -1;
    }

    printf("[I] Client sending INIT\n");

    c2s_msg_t msg;
    msg.type = MESSAGE_INIT;
    msg.data.init.client_queue = client->client_queue;
    int err = msgsnd(client->server_queue, &msg, sizeof(msg.data), 0);
    if (err)
    {
        perror("[E] Error sending INIT");
        return -1;
    }

    printf("[I] OK\n");
    return 0;
}


int client_loop(client_t *client)
{
    if (client->client_queue == -1)
    {
        printf("[E] Cannot start loop, client queue not opened\n");
        return -1;
    }

    if (client->server_queue == -1)
    {
        printf("[E] Cannot start loop, server queue not opened\n");
        return -1;
    }

    printf("[I] Starting client loop\n");

    while (!g_should_stop)
    {
        s2c_msg_t msg;
        ssize_t n_read = msgrcv(client->client_queue, &msg, sizeof(msg.data), 0, 0);
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
                client_handle_init(client, &msg.data.init);
                break;

            case MESSAGE_STOP:
                printf("[I] Got STOP message\n");
                client_handle_stop(client);
                break;

            default:
                printf("[E] Got unknown message: %ld\n", msg.type);
                break;
        }
    }

    printf("[I] Client loop done\n");
    return 0;
}

int client_handle_init(client_t *client, struct s2c_init_msg_t *msg)
{
    client->client_id = msg->client_id;
    printf("[I] Client was assigned ID %d\n", client->client_id);
    return 0;
}

int client_send_stop(client_t *client)
{
    if (client->client_id == -1)
    {
        printf("[E] Cannot send STOP, client ID not assigned\n");
        return -1;
    }

    if (client->server_queue == -1)
    {
        printf("[E] Cannot send STOP, server queue not opened\n");
        return -1;
    }

    printf("[I] Client sending STOP\n");

    c2s_msg_t msg;
    msg.type = MESSAGE_STOP;
    msg.data.stop.client_id = client->client_id;
    int err = msgsnd(client->server_queue, &msg, sizeof(msg.data), 0);
    if (err)
    {
        perror("[E] Error sending STOP");
        return -1;
    }

    client->client_id = -1;
    client->server_queue = -1;

    printf("[I] OK\n");
    return 0;
}

int client_handle_stop(client_t *client)
{
    printf("[I] Client got STOP\n");
    g_should_stop = true;
    return 0;
}
