#include "game_manager.h"

const char* gman_err_msg(gman_err_t err)
{
    switch (err)
    {
        case GMAN_OK:
            return "OK";

        case GMAN_ERR:
            return "Error";
    }

    return "Error";
}

void gman_init(gman_t *man)
{

}

gman_err_t gman_add_player(gman_t *man, const char *name)
{
    return GMAN_ERR;
}

gman_err_t gman_execute_move(gman_t *man, const char *name, pos_t move)
{
    return GMAN_ERR;
}
