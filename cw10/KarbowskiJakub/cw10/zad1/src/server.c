#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#include "log.h"
#include "packet.h"

err_t server_open(server_t *server, short port)
{
    if (!server) return ERR_GENERIC;

    server->netsock = -1;

    gman_init(&server->game_manager);

    for (int i = 0; i < SERVER_MAX_CONNECTIONS; ++i)
    {
        server->connections[i].status = CONNECTION_STATUS_DISCONNECTED;
        server->connections[i].sock = -1;
        server->connections[i].recv_count = 0;
    }

    if (server_open_net_sock(server, port))
    {
        LOGE("Could not open net sock");
        return ERR_GENERIC;
    }

    return ERR_OK;
}

void server_close(server_t *server)
{
    if (!server) return;

    if (server->netsock != -1)
        close(server->netsock);

    for (int i = 0; i < SERVER_MAX_CONNECTIONS; ++i)
    {
        if (server->connections[i].status == CONNECTION_STATUS_ACTIVE)
        {
            close(server->connections[i].sock);
        }
    }
}

err_t server_loop(server_t *server)
{
    if (!server) return ERR_GENERIC;

    if (server->netsock == -1) return ERR_GENERIC;

    struct pollfd fds[SERVER_MAX_CONNECTIONS + 1];
    fds[SERVER_MAX_CONNECTIONS].fd = server->netsock;
    fds[SERVER_MAX_CONNECTIONS].events = POLLIN;

    for (;;)
    {
        for (int i = 0; i < SERVER_MAX_CONNECTIONS; ++i)
        {
            if (server->connections[i].status == CONNECTION_STATUS_ACTIVE)
            {
                fds[i].fd = server->connections[i].sock;
                fds[i].events = POLLIN;
            }
            else
            {
                fds[i].fd = -1;
                fds[i].events = 0;
            }
        }

        if (poll(fds, sizeof(fds) / sizeof(*fds), -1) > 0)
        {
            if (fds[SERVER_MAX_CONNECTIONS].revents & POLLIN)
            {
                LOGI("New connection");

                int sock = accept(server->netsock, NULL, NULL);
                if (sock == -1)
                {
                    perror("Could not accept connection");
                }
                else
                {
                    if (!server_add_connection(server, sock))
                        LOGI("Accepted connection");
                    else
                        LOGE("Could not accept connection");
                }
            }

            for (int i = 0; i < SERVER_MAX_CONNECTIONS; ++i)
            {
                server_client_conn_t *conn = &server->connections[i];

                if (fds[i].revents & POLLIN)
                {
                    LOGI("Data on connection %d", i);

                    if (conn->recv_count == 0)
                    {
                        int n = (int) read(
                            conn->sock,
                            conn->recv_buf,
                            1
                        );

                        if (n != 1)
                        {
                            LOGE("Socket read error");
                            server_kill_client(server, i);
                        }
                        else
                        {
                            conn->recv_count = 1;
                            LOGI("Got packet length %d", conn->recv_buf[0]);
                        }
                    }
                    else
                    {
                        int n = (int) read(
                            conn->sock,
                            conn->recv_buf + conn->recv_count,
                            conn->recv_buf[0] - conn->recv_count
                        );

                        if (n <= 0)
                        {
                            LOGE("Socket read error");
                            server_kill_client(server, i);
                        }
                        else
                        {
                            conn->recv_count += n;
                            LOGI("Got %d/%d bytes", conn->recv_count, conn->recv_buf[0]);

                            if (conn->recv_count == conn->recv_buf[0])
                            {
                                LOGI("Got full packet");

                                packet_t packet;
                                packet_parse(conn->recv_buf, &packet);

                                if (server_handle_packet(server, i, &packet))
                                {
                                    LOGE("Packet handling error");
                                }

                                conn->recv_count = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

err_t server_open_net_sock(server_t *server, short port)
{
    if (!server) return ERR_GENERIC;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Could not create socket");
        return ERR_GENERIC;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*) &addr, sizeof addr))
    {
        perror("Could not bind socket");
        close(sock);
        return ERR_GENERIC;
    }

    if (listen(sock, 32))
    {
        perror("Could not bind socket");
        close(sock);
        return ERR_GENERIC;
    }

    printf("Socket opened on 0.0.0.0:%d\n", ntohs(addr.sin_port));

    server->netsock = sock;

    return ERR_OK;
}

err_t server_handle_packet(server_t *server, int con, const packet_t *packet)
{
    switch (packet->type)
    {
        case PACKET_INIT:
            return server_handle_init(server, con, &packet->init);

        case PACKET_MOVE:
            return server_handle_move(server, con, &packet->move);

        case PACKET_GAME:
            LOGE("Server got game packet! (should never happen)");
            break;

        case PACKET_STATUS:
            LOGI("Got status %d: %s", packet->status.err, err_msg(packet->status.err));
            break;
    }

    return ERR_OK;
}

err_t server_add_connection(server_t *server, int sock)
{
    for (int i = 0; i < SERVER_MAX_CONNECTIONS; ++i)
    {
        if (server->connections[i].status == CONNECTION_STATUS_DISCONNECTED)
        {
            server->connections[i].status = CONNECTION_STATUS_ACTIVE;
            server->connections[i].sock = sock;
            server->connections[i].recv_count = 0;
            return ERR_OK;
        }
    }
    LOGE("Out of connections");
    return ERR_GENERIC;
}

err_t server_handle_init(server_t *server, int con, const init_packet_t *packet)
{
    if (!server || !packet) return ERR_GENERIC;

    err_t err = gman_add_player(&server->game_manager, packet->name);

    if (err) LOGE("Failed to add player");
    else LOGI("Added new player");

    packet_t resp;
    resp.type = PACKET_STATUS;
    resp.status.err = err;
    if (packet_send(server->connections[con].sock, &resp))
        LOGE("Failed sending response");

    return ERR_OK;
}

err_t server_handle_move(server_t *server, int con, const move_packet_t *packet)
{
    if (!server || !packet) return ERR_GENERIC;

    err_t err = gman_execute_move(&server->game_manager, packet->name, packet->pos);

    if (err) LOGE("Failed executing move");
    else LOGI("Executed move");

    packet_t resp;
    resp.type = PACKET_STATUS;
    resp.status.err = err;
    if (packet_send(server->connections[con].sock, &resp))
        LOGE("Failed sending response");

    return ERR_OK;
}

void server_kill_client(server_t *server, int con)
{
    if (!server) return;
    if (server->connections[con].sock != -1)
    {
        close(server->connections[con].sock);
        LOGI("Killed client %i", con);
    }
}
