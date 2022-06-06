#include "err.h"

const char* err_msg(err_t err)
{
    switch (err)
    {
        case ERR_OK:
            return "OK";

        case ERR_GENERIC:
            return "Error";

        case ERR_NAME_OCCUPIED:
            return "Name is already occupied";

        case ERR_PLAYER_NOT_FOUND:
            return "Player not found";

        default:
            return "Unknown error";
    }
}
