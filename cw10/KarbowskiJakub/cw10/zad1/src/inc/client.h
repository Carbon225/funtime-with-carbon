#ifndef JK_10_01_CLIENT_H
#define JK_10_01_CLIENT_H

#include <stdint.h>

#include "game.h"
#include "packet.h"
#include "err.h"

typedef enum connection_type_t
{
    CONNECTION_NET,
    CONNECTION_UNIX,
} connection_type_t;

typedef struct client_session_t
{
    int sock;
    game_t game;
} client_session_t;

err_t client_connect(client_session_t *session,
                   connection_type_t connection_type,
                   const char *address);

err_t client_disconnect(client_session_t *session);

err_t client_get_response(client_session_t *session);

err_t client_send_init(client_session_t *session, const char *name);

err_t client_get_game(client_session_t *session);

err_t client_send_move(client_session_t *session, pos_t pos);

#endif
