#include <stdio.h>

static const char HELP[] =
        "SO Lab5 Zad3 - Jakub Karbowski\n"
        "Usage:\n"
        "%s FIFO LINE INPUT N\n"
        "  FIFO  - path to fifo\n"
        "  LINE  - line number\n"
        "  INPUT - path to input file\n"
        "  N     - read burst size\n";

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    return 0;
}
