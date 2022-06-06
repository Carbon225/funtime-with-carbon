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
        perror("Could not create socket");
        return ERR_GENERIC;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    LOGI("Connecting to 127.0.0.1:8080");

    if (connect(sockfd, (const struct sockaddr*) &addr, sizeof addr) != 0)
    {
        perror("Could not connect to server");
        close(sockfd);
        return ERR_GENERIC;
    }

    LOGI("Connected\n");

    session->sock = sockfd;

    return ERR_OK;
}

err_t client_get_game(client_session_t *session)
{
    packet_t packet;
    packet_receive(session->sock, &packet);

    if (packet.type == PACKET_STATUS)
    {
        LOGE("From server: %s", err_msg(packet.status.err));
        return ERR_GENERIC;
    }

    if (packet.type != PACKET_GAME)
    {
        LOGE("Invalid response from server");
        return ERR_GENERIC;
    }

    LOGI("Got game from server");

    session->game = packet.game.game;

    return ERR_OK;
}

err_t client_send_init(client_session_t *session, const char *name)
{
    return ERR_GENERIC;
}

err_t client_send_move(client_session_t *session, pos_t pos)
{
    return ERR_GENERIC;
}

err_t client_disconnect(client_session_t *session)
{
    return ERR_GENERIC;
}

err_t client_get_response(client_session_t *session)
{
    return ERR_GENERIC;
}
