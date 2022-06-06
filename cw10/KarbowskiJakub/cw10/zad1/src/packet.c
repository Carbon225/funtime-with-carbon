#include "packet.h"

#include <stdint.h>
#include <string.h>

void create_packet(void *buf, const packet_t *packet)
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
            memcpy((char*)(b + 3), packet->status.msg, STATUS_MESSAGE_MAX);
            b[0] = 3 + STATUS_MESSAGE_MAX;
            break;
    }
}

void parse_packet(const void *buf, packet_t *packet)
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
            memcpy(packet->status.msg, (char*)(b + 3), STATUS_MESSAGE_MAX);
            break;
    }
}
