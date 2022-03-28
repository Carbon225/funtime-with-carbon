#include <stdio.h>
#include <stdlib.h>
#include "fork_search.h"

static const char HELP[] =
        "SO Lab3 Zad3 - Jakub Karbowski\n"
        "Usage:\n"
        "%s DIR PATTERN DEPTH - search DIR for files containing PATTERN "
        "with maximum search depth DEPTH\n";

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    char *endptr;
    long depth = strtol(argv[3], &endptr, 10);
    if (*endptr || depth < 0)
    {
        fprintf(stderr, "Invalid depth: %s\n", argv[3]);
        return -1;
    }

    int err = fork_search(argv[1], argv[2], depth);
    if (err) fprintf(stderr, "Error!\n");

    return err;
}
