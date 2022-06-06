#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "client.h"
#include "game.h"
#include "log.h"
#include "err.h"

static const char HELP[] =
        "SO Lab 10 - Jakub Karbowski\n"
        "Usage:\n"
        "%s NAME net|unix ADDRESS\n";

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    const char *client_name = argv[1];

    connection_type_t connection_type;
    if (!strcmp("net", argv[2]))
    {
        connection_type = CONNECTION_NET;
    }
    else if (!strcmp("unix", argv[2]))
    {
        connection_type = CONNECTION_UNIX;
    }
    else
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    const char *server_address = argv[3];

    client_session_t session;
    if (client_connect(&session, connection_type, server_address))
    {
        LOGE("Could not connect to server");
        return -1;
    }

    LOGI("Connected");

    err_t res = client_log_in(&session, client_name);
    if (res)
    {
        LOGE("Error logging in: %s", err_msg(res));
        client_disconnect(&session);
        return -1;
    }

    LOGI("Logged in");

    for (;;)
    {
        game_t game;
        player_t player;
        if (client_get_game(&session, &game, &player))
        {
            LOGE("Error getting game");
            break;
        }

        game_print(&game);
        printf("You are %c\n", player == PLAYER_X ? 'X' : 'O');

        if (game.next_player == player)
        {
            printf("Enter your move (1-9):\n");
            int c;
            do
            {
                c = fgetc(stdin);
            } while (c != '1' && c != '2' && c != '3' &&
                     c != '4' && c != '5' && c != '6' &&
                     c != '7' && c != '8' && c != '9');
            while (fgetc(stdin) != '\n');
            pos_t pos = (pos_t) (c - '1');

            res = client_send_move(&session, pos);
            if (res)
            {
                LOGE("Move error: %s", err_msg(res));
                break;
            }
        }
    }

    return 0;
}
