#include "packet.h"

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "log.h"

void packet_create(void *buf, const packet_t *packet)
{
    if (!buf || !packet) return;

    uint8_t *b = buf;

    b[1] = packet->type;

    switch (packet->type)
    {
        case PACKET_INIT:
            memcpy((char*)(b + 2), packet->init.name, PLAYER_NAME_MAX);
            b[0] = 2 + PLAYER_NAME_MAX;
            break;

        case PACKET_MOVE:
            b[2] = packet->move.pos;
            memcpy((char*)(b + 3), packet->move.name, PLAYER_NAME_MAX);
            b[0] = 3 + PLAYER_NAME_MAX;
            break;

        case PACKET_GAME:
            b[2] = packet->game.game.board[0];
            b[3] = packet->game.game.board[1];
            b[4] = packet->game.game.board[2];

            b[5] = packet->game.game.board[3];
            b[6] = packet->game.game.board[4];
            b[7] = packet->game.game.board[5];

            b[8] = packet->game.game.board[6];
            b[9] = packet->game.game.board[7];
            b[10] = packet->game.game.board[8];

            b[11] = packet->game.game.next_player;

            b[12] = packet->game.game.winner;

            b[13] = packet->game.game.is_over;

            b[0] = 14;
            break;

        case PACKET_STATUS:
            b[2] = packet->status.err;
            b[0] = 3;
            break;
    }
}

void packet_parse(const void *buf, packet_t *packet)
{
    if (!buf || !packet) return;

    const uint8_t *b = buf;

    packet->type = b[1];

    switch (packet->type)
    {
        case PACKET_INIT:
            memcpy(packet->init.name, (char*)(b + 2), PLAYER_NAME_MAX);
            break;

        case PACKET_MOVE:
            packet->move.pos = b[2];
            memcpy(packet->move.name, (char*)(b + 3), PLAYER_NAME_MAX);
            break;

        case PACKET_GAME:
            packet->game.game.board[0] = b[2];
            packet->game.game.board[1] = b[3];
            packet->game.game.board[2] = b[4];

            packet->game.game.board[3] = b[5];
            packet->game.game.board[4] = b[6];
            packet->game.game.board[5] = b[7];

            packet->game.game.board[6] = b[8];
            packet->game.game.board[7] = b[9];
            packet->game.game.board[8] = b[10];

            packet->game.game.next_player = b[11];

            packet->game.game.winner = b[12];

            packet->game.game.is_over = b[13];
            break;

        case PACKET_STATUS:
            packet->status.err = b[2];
            break;
    }
}

int packet_send(int fd, const packet_t *packet)
{
    if (!packet) return -1;
    uint8_t buf[PACKET_MAX_SIZE];
    packet_create(buf, packet);
    return write(fd, buf, buf[0]) == buf[0] ? 0 : -1;
}

int packet_receive(int fd, packet_t *packet)
{
    if (!packet) return -1;

    uint8_t buf[PACKET_MAX_SIZE];

    if (read(fd, buf, 1) != 1)
    {
        LOGE("Socket read error");
        return -1;
    }

    int count = 1;
    LOGI("Got packet length %d", buf[0]);

    while (count < buf[0])
    {
        int n = (int) read(fd, buf + count, buf[0] - count);
        if (n <= 0)
        {
            LOGE("Socket read error");
            return -1;
        }
        count += n;
        LOGI("Got %d/%d bytes", count, buf[0]);
    }

    LOGI("Got full packet");
    packet_parse(buf, packet);

    return 0;
}
