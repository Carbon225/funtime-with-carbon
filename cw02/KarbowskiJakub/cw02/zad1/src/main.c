#include <stdio.h>
#include "libcopy.h"

static const char HELP[] =
        "SO Lab 2 - Jakub Karbowski\n";

int main(int argc, char **argv)
{
    if (argc != 3 && argc != 1)
    {
        fprintf(stderr, HELP);
        return -1;
    }

    if (argc == 3)
    {
        const char *in_file = argv[1];
        const char *out_file = argv[2];
        return copy_file(in_file, out_file);
    }

    return 0;
}
