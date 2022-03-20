#include <stdio.h>
#include "libcount.h"

static const char HELP[] =
        "SO Lab2 Zad2 - Jakub Karbowski\n"
        "Usage:\n"
        "%s CHAR FILE - count CHARs in FILE\n";

int main(int argc, char **argv)
{
    int err = 0;

    if (argc != 3)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    // first argument has to be single char
    if (argv[1][0] == 0 || argv[1][1] != 0)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    char c = argv[1][0];
    const char *in_file = argv[2];
    char_stats_t stats;
    err = count_chars(in_file, c, &stats);
    if (!err)
    {
        printf("Character occurrences:      %d\n", stats.n_chars);
        printf("Lines containing character: %d\n", stats.n_lines);
    }
    else fprintf(stderr, "Error!\n");

    return err;
}
