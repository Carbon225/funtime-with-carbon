#include "err.h"

const char* err_msg(err_t err)
{
    switch (err)
    {
        case ERR_OK:
            return "OK";

        case ERR_GENERIC:
            return "Error";
    }

    return "Unknown error";
}
