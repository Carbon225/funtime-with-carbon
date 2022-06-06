#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdint.h>
#include <netinet/in.h>
#include <poll.h>

#include "log.h"
#include "packet.h"
#include "err.h"

err_t client_connect(client_session_t *session,
                     connection_type_t connection_type,
                     const char *address)
{
    if (!session || !address) return ERR_GENERIC;

    session->sock = -1;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        LOGE("Could not create socket");
        return ERR_GENERIC;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sockfd, (const struct sockaddr*) &addr, sizeof addr) != 0)
    {
        LOGE("Could not connect to server");
        close(sockfd);
        return ERR_GENERIC;
    }

    session->sock = sockfd;

    return ERR_OK;
}

err_t client_log_in(client_session_t *session, const char *name)
{
    if (!session || !name) return ERR_GENERIC;
    packet_t packet;
    packet.type = PACKET_INIT;
    memcpy(packet.init.name, name, PLAYER_NAME_MAX);
    memcpy(session->name, name, PLAYER_NAME_MAX);
    if (packet_send(session->sock, &packet))
    {
        LOGE("Error sending init");
        return ERR_GENERIC;
    }
    return client_get_response(session);
}

err_t client_send_move(client_session_t *session, pos_t pos)
{
    if (!session) return ERR_GENERIC;
    packet_t packet;
    packet.type = PACKET_MOVE;
    memcpy(packet.move.name, session->name, PLAYER_NAME_MAX);
    packet.move.pos = pos;
    if (packet_send(session->sock, &packet))
    {
        LOGE("Error sending move");
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

    packet_t packet;
    packet_receive(session->sock, &packet);

    if (packet.type != PACKET_GAME)
    {
        LOGE("Invalid response from server");
        return ERR_GENERIC;
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
}

err_t client_get_response(client_session_t *session)
{
    if (!session) return ERR_GENERIC;

    packet_t packet;
    packet_receive(session->sock, &packet);

    if (packet.type != PACKET_STATUS)
    {
        LOGE("Invalid response from server");
        return ERR_GENERIC;
    }

    LOGI("Got status from server: %s", err_msg(packet.status.err));

    return packet.status.err;
}
