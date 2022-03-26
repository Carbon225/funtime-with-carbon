#include <stdio.h>
#include "fork_search.h"

static const char HELP[] =
        "SO Lab3 Zad3 - Jakub Karbowski\n"
        "Usage:\n"
        "%s DIR PATTERN - search DIR for files containing PATTERN\n";

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    int err = fork_search(argv[1], argv[2]);
    if (err) fprintf(stderr, "Error!\n");

    return err;
}
