#include <stdio.h>

#include "log.h"
#include "server.h"

int main(int argc, char **argv)
{
    server_t server;

    if (server_open(&server, 8080))
    {
        LOGE("Could not open server");
        return -1;
    }

    if (server_loop(&server))
    {
        LOGE("Server loop error");
    }

    server_close(&server);

    return 0;
}
