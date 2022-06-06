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

int client_connect(client_session_t *session,
                   const char *name,
                   connection_type_t connection_type,
                   const char *address)
{
    if (!session || !name || !address) return -1;

    session->sock = -1;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Could not create socket");
        return -1;
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
        return -1;
    }

    LOGI("Connected\n");

    session->sock = sockfd;

    return 0;
}

int client_get_game(client_session_t *session)
{
    packet_t packet;
    client_recv_packet(session, &packet);

    if (packet.type == PACKET_STATUS)
    {
        LOGE("From server: %s", packet.status.msg);
        return -1;
    }

    if (packet.type != PACKET_GAME)
    {
        LOGE("Invalid response from server");
        return -1;
    }

    LOGI("Got game from server");

    session->game = packet.game.game;

    return 0;
}

int client_send_move(client_session_t *session, pos_t pos)
{
    return -1;
}

int client_recv_packet(client_session_t *session, packet_t *packet)
{
    if (!session || !packet) return -1;

    uint8_t buf[PACKET_MAX_SIZE];

    int n = (int) read(
        session->sock,
        buf,
        1
    );

    if (n != 1)
    {
        LOGE("Socket read error");
        return -1;
    }

    int count = 1;
    LOGI("Got packet length %d", buf[0]);

    while (count < buf[0])
    {
        n = (int) read(
            session->sock,
            buf + count,
            buf[0] - count
        );

        if (n <= 0)
        {
            LOGE("Socket read error");
            return -1;
        }

        count += n;
        LOGI("Got %d/%d bytes", count, buf[0]);
    }

    LOGI("Got full packet");

    parse_packet(buf, packet);

    return 0;
}
