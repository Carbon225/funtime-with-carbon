#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "server.h"

static const char HELP[] =
        "SO Lab 10 - Jakub Karbowski\n"
        "Usage:\n"
        "%s TCP_PORT UNIX_SOCK_PATH\n";


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    char *endptr;
    long port = strtol(argv[1], &endptr, 10);
    if (!*argv[1] || *endptr || port < 1 || port > INT16_MAX)
    {
        fprintf(stderr, "Invalid port\n");
        return -1;
    }

    const char *sock_path = argv[2];

    server_t server;

    if (server_open(&server, (short) port, sock_path))
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
