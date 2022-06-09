#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <poll.h>
#include <arpa/inet.h>

#include "log.h"
#include "packet.h"
#include "err.h"

err_t client_connect(client_session_t *session,
                     connection_type_t connection_type,
                     const char *address)
{
    if (!session || !address) return ERR_GENERIC;

    session->sock = -1;
    session->connected = false;
    session->un_active = false;

    int sockfd;
    const struct sockaddr *addr;
    socklen_t socklen;
    struct sockaddr_in addr_in;
    struct sockaddr_un addr_un;

    if (connection_type == CONNECTION_NET)
    {
        int col_i = -1;
        for (int i = 0; address[i]; ++i)
        {
            if (address[i] == ':')
            {
                col_i = i;
                break;
            }
        }
        if (col_i == -1 || col_i > 50)
        {
            LOGE("Invalid address");
            return ERR_GENERIC;
        }

        char addr_part[100];
        memcpy(addr_part, address, col_i);
        addr_part[col_i] = 0;

        char *endptr;
        long port = strtol(&address[col_i + 1], &endptr, 10);
        if (!address[col_i + 1] || *endptr || port < 1 || port > INT16_MAX)
        {
            LOGE("Invalid port");
            return ERR_GENERIC;
        }

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            LOGE("Could not create socket");
            return ERR_GENERIC;
        }

        memset(&addr_in, 0, sizeof addr_in);
        addr_in.sin_family = AF_INET;
        addr_in.sin_port = htons((short) port);
        inet_aton(addr_part, &addr_in.sin_addr);

        addr = (struct sockaddr*) &addr_in;
        socklen = sizeof(addr_in);
    }
    else
    {
        sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            LOGE("Could not create socket");
            return ERR_GENERIC;
        }

        {
            struct sockaddr_un addr;
            memset(&addr, 0, sizeof addr);
            addr.sun_family = AF_UNIX;
            sprintf(session->un_path, "%ld", random());
            strncpy(addr.sun_path, session->un_path, sizeof(addr.sun_path) - 1);

            if (bind(sockfd, (struct sockaddr*) &addr, sizeof addr))
            {
                perror("Could not bind socket");
                close(sockfd);
                return ERR_GENERIC;
            }

            session->un_active = true;
        }

        memset(&addr_un, 0, sizeof addr_un);
        addr_un.sun_family = AF_UNIX;
        strncpy(addr_un.sun_path, address, sizeof(addr_un.sun_path) - 1);

        addr = (struct sockaddr*) &addr_un;
        socklen = sizeof(addr_un);
    }

    if (connect(sockfd, addr, socklen) != 0)
    {
        LOGE("Could not connect to server");
        close(sockfd);
        return ERR_GENERIC;
    }

    session->sock = sockfd;
    session->connected = true;

    return ERR_OK;
}

err_t client_log_in(client_session_t *session, const char *name)
{
    if (!session || !name) return ERR_GENERIC;

    if (!session->connected) return ERR_GENERIC;

    packet_t packet;
    packet.type = PACKET_INIT;
    memcpy(packet.init.name, name, PLAYER_NAME_MAX);
    memcpy(session->name, name, PLAYER_NAME_MAX);
    if (packet_send(session->sock, NULL, 0, &packet))
    {
        LOGE("Error sending init");
        session->connected = false;
        return ERR_GENERIC;
    }
    return client_get_response(session);
}

err_t client_send_move(client_session_t *session, pos_t pos)
{
    if (!session) return ERR_GENERIC;

    if (!session->connected) return ERR_GENERIC;

    packet_t packet;
    packet.type = PACKET_MOVE;
    memcpy(packet.move.name, session->name, PLAYER_NAME_MAX);
    packet.move.pos = pos;
    if (packet_send(session->sock, NULL, 0, &packet))
    {
        LOGE("Error sending move");
        session->connected = false;
        return ERR_GENERIC;
    }
    return client_get_response(session);
}

err_t client_get_game(client_session_t *session,
                      game_t *game,
                      player_t *player,
                      char *opponent)
{
    if (!session || !game || !player || !opponent)
        return ERR_GENERIC;

    if (!session->connected) return ERR_GENERIC;

    packet_t packet;

    for (;;)
    {
        if (packet_receive(session->sock, NULL, NULL, &packet))
        {
            LOGE("Error receiving game");
            session->connected = false;
            return ERR_GENERIC;
        }

        if (packet.type == PACKET_PING)
        {
            if (packet_send(session->sock, NULL, 0, &packet))
            {
                session->connected = false;
                return ERR_GENERIC;
            }
            continue;
        }

        if (packet.type != PACKET_GAME)
        {
            LOGE("Invalid response from server");
            return ERR_GENERIC;
        }

        break;
    }

    LOGI("Got game from server");

    *game = packet.game.game;
    *player = packet.game.player;
    memcpy(opponent, packet.game.opponent, PLAYER_NAME_MAX);

    return ERR_OK;
}

void client_disconnect(client_session_t *session)
{
    if (!session) return;
    if (session->sock != -1)
        close(session->sock);
    session->connected = false;
    if (session->un_active)
        unlink(session->un_path);
}

err_t client_get_response(client_session_t *session)
{
    if (!session) return ERR_GENERIC;

    if (!session->connected) return ERR_GENERIC;

    packet_t packet;

    for (;;)
    {
        if (packet_receive(session->sock, NULL, NULL, &packet))
        {
            LOGE("Error receiving response");
            session->connected = false;
            return ERR_GENERIC;
        }

        if (packet.type == PACKET_PING)
        {
            if (packet_send(session->sock, NULL, 0, &packet))
            {
                session->connected = false;
                return ERR_GENERIC;
            }
            continue;
        }

        if (packet.type != PACKET_STATUS)
        {
            LOGE("Invalid response from server");
            return ERR_GENERIC;
        }

        break;
    };

    LOGI("Got status from server: %s", err_msg(packet.status.err));

    return packet.status.err;
}
