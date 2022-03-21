#include <stdio.h>
#include "walkdir.h"

static const char HELP[] =
        "SO Lab2 Zad3 - Jakub Karbowski\n"
        "Usage:\n"
        "%s DIR - walk DIR and print statistics\n";

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    dir_stats_t stats;
    printf("Walking %s...\n", argv[1]);
    int err = walk_dir(argv[1], &stats);
    if (err)
    {
        fprintf(stderr, "Error!\n");
        return -1;
    }

    printf(
        "\nSummary:\n"
        "reg:  %d\n"
        "dir:  %d\n"
        "chr:  %d\n"
        "blk:  %d\n"
        "fifo: %d\n"
        "link: %d\n"
        "sock: %d\n",
        stats.n_reg,
        stats.n_dir,
        stats.n_chr,
        stats.n_blk,
        stats.n_fifo,
        stats.n_link,
        stats.n_sock
    );

    return 0;
}