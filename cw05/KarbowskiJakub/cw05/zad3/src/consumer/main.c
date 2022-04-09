#include <stdio.h>

static const char HELP[] =
        "SO Lab5 Zad3 - Jakub Karbowski\n"
        "Usage:\n"
        "%s FIFO OUTPUT N\n"
        "  FIFO  - path to fifo\n"
        "  INPUT - path to input file\n"
        "  N     - read burst size\n";

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    return 0;
}
