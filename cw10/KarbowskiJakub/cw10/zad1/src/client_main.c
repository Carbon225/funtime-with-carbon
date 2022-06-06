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

    if (client_send_init(&session, client_name))
    {
        LOGE("Failed sending init");
        client_disconnect(&session);
        return -1;
    }

    err_t ret = client_get_response(&session);
    if (ret)
    {
        LOGE("Error from server: %s", err_msg(ret));
        client_disconnect(&session);
        return -1;
    }

    for (;;)
    {
        client_get_game(&session);
        game_print(&session.game);

        printf("Enter your move (1-9):\n");
        int c;
        do
        {
            c = fgetc(stdin);
        } while (c == '\n');
        while (fgetc(stdin) != '\n');
        pos_t pos = (pos_t) (c - '1');

        client_send_move(&session, pos);

        client_get_game(&session);
        game_print(&session.game);
    }

    return 0;
}
