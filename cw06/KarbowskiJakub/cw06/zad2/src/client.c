#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

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

    client_close_server(&client);

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

    struct mq_attr attr;
    attr.mq_maxmsg = 8;
    attr.mq_msgsize = sizeof(s2c_msg_t);

    for (int i = 0; i < 100; ++i)
    {
        char buf[128];
        client->client_queue_uid = random();
        sprintf(buf, CLIENT_QUEUE_PATH_TEMPLATE, client->client_queue_uid);
        client->client_queue = mq_open(buf, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);
        if (client->client_queue != -1) break;
    }

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

    mq_close(client->client_queue);
    char buf[128];
    sprintf(buf, CLIENT_QUEUE_PATH_TEMPLATE, client->client_queue_uid);
    mq_unlink(buf);

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

    client->server_queue = mq_open(SERVER_QUEUE_PATH, O_WRONLY);

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
    msg.data.init.client_queue_uid = client->client_queue_uid;
    int err = mq_send(client->server_queue, (void*) &msg, sizeof(msg), MESSAGE_INIT);
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
        struct pollfd fds[2];
        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;
        fds[1].fd = client->client_queue;
        fds[1].events = POLLIN;

        if (poll(fds, 2, 0) > 0)
        {
            if (fds[0].revents & POLLIN)
            {
                char buf[MESSAGE_MAX_BODY_SIZE];
                if (fgets(buf, sizeof buf, stdin) == buf)
                {
                    if (buf[strlen(buf) - 1] == '\n')
                        buf[strlen(buf) - 1] = 0;

                    char cmd[64] = {0};
                    sscanf(buf, "%s", cmd);

                    if (!strcmp("LIST", cmd))
                    {
                        client_send_list(client);
                    }
                    else if (!strcmp("2ALL", cmd))
                    {
                        char msg[MESSAGE_MAX_BODY_SIZE + 1];
                        if (sscanf(buf, "%s %s", cmd, msg) == 2)
                            client_send_2all(client, msg);
                    }
                    else if (!strcmp("2ONE", cmd))
                    {
                        char msg[MESSAGE_MAX_BODY_SIZE + 1];
                        int recipient_id;
                        if (sscanf(buf, "%s %d %s", cmd, &recipient_id, msg) == 3)
                            client_send_2one(client, recipient_id, msg);
                    }
                    else if (!strcmp("STOP", cmd))
                    {
                        g_should_stop = true;
                    }
                    else
                    {
                        printf("[E] Invalid command: %s\n", cmd);
                    }
                }
            }

            if (fds[1].revents & POLLIN)
            {
                s2c_msg_t msg;
                ssize_t n_read = mq_receive(client->client_queue, (void*) &msg, sizeof(msg), NULL);
                if (n_read == -1)
                {
                    perror("[E] Error receiving message");
                }
                else
                {
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

                        case MESSAGE_LIST:
                            printf("[I] Got LIST message\n");
                            client_handle_list(client, &msg.data.list);
                            break;

                        case MESSAGE_2ONE:
                            printf("[I] Got MAIL message\n");
                            client_handle_mail(client, &msg.data.mail);
                            break;

                        default:
                            printf("[E] Got unknown message: %ld\n", msg.type);
                            break;
                    }
                }
            }
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
    int err = mq_send(client->server_queue, (void*) &msg, sizeof(msg), MESSAGE_STOP);
    if (err)
    {
        perror("[E] Error sending STOP");
        return -1;
    }

    client->client_id = -1;

    printf("[I] OK\n");
    return 0;
}

int client_handle_stop(client_t *client)
{
    printf("[I] Client got STOP\n");
    g_should_stop = true;
    return 0;
}

int client_send_list(client_t *client)
{
    if (client->client_id == -1)
    {
        printf("[E] Cannot send LIST, client ID not assigned\n");
        return -1;
    }

    if (client->server_queue == -1)
    {
        printf("[E] Cannot send LIST, server queue not opened\n");
        return -1;
    }

    printf("[I] Client sending LIST\n");

    c2s_msg_t msg;
    msg.type = MESSAGE_LIST;
    msg.data.list.client_id = client->client_id;
    int err = mq_send(client->server_queue, (void*) &msg, sizeof(msg), MESSAGE_LIST);
    if (err)
    {
        perror("[E] Error sending LIST");
        return -1;
    }

    printf("[I] OK\n");
    return 0;
}

int client_send_2all(client_t *client, const char body[])
{
    if (client->client_id == -1)
    {
        printf("[E] Cannot send 2ALL, client ID not assigned\n");
        return -1;
    }

    if (client->server_queue == -1)
    {
        printf("[E] Cannot send 2ALL, server queue not opened\n");
        return -1;
    }

    printf("[I] Client sending 2ALL\n");

    c2s_msg_t msg;
    msg.type = MESSAGE_2ALL;
    msg.data.to_all.client_id = client->client_id;
    strncpy(msg.data.to_all.body, body, MESSAGE_MAX_BODY_SIZE);
    msg.data.to_all.body[MESSAGE_MAX_BODY_SIZE] = 0;
    int err = mq_send(client->server_queue, (void*) &msg, sizeof(msg), MESSAGE_2ALL);
    if (err)
    {
        perror("[E] Error sending 2ALL");
        return -1;
    }

    printf("[I] OK\n");
    return 0;
}

int client_send_2one(client_t *client, int recipient_id, const char body[])
{
    if (client->client_id == -1)
    {
        printf("[E] Cannot send 2ONE, client ID not assigned\n");
        return -1;
    }

    if (client->server_queue == -1)
    {
        printf("[E] Cannot send 2ONE, server queue not opened\n");
        return -1;
    }

    printf("[I] Client sending 2ONE\n");

    c2s_msg_t msg;
    msg.type = MESSAGE_2ONE;
    msg.data.to_one.client_id = client->client_id;
    msg.data.to_one.recipient_id = recipient_id;
    strncpy(msg.data.to_one.body, body, MESSAGE_MAX_BODY_SIZE);
    msg.data.to_one.body[MESSAGE_MAX_BODY_SIZE] = 0;
    int err = mq_send(client->server_queue, (void*) &msg, sizeof(msg), MESSAGE_2ONE);
    if (err)
    {
        perror("[E] Error sending 2ONE");
        return -1;
    }

    printf("[I] OK\n");
    return 0;
}

int client_handle_list(client_t *client, struct s2c_list_msg_t *msg)
{
    printf("Found client: %d\n", msg->client_id);
    return 0;
}

int client_handle_mail(client_t *client, struct s2c_mail_msg_t *msg)
{
    printf("%s\tGot message from %d: %s\n", ctime(&msg->time), msg->sender_id, msg->body);
    return 0;
}

int client_close_server(client_t *client)
{
    printf("[I] Closing server\n");

    if (client->server_queue == -1)
    {
        printf("[E] Cannot close server, not opened\n");
        return -1;
    }

    mq_close(client->server_queue);

    printf("[I] OK\n");

    return 0;
}
