#ifndef JK_02_03_WALKDIR_H
#define JK_02_03_WALKDIR_H

#if !defined(IMPL_STAT) && !defined(IMPL_NFTW)
#define IMPL_STAT
#endif

typedef struct dir_stats_t
{
    int n_fifo;
    int n_chr;
    int n_dir;
    int n_blk;
    int n_reg;
    int n_link;
    int n_sock;
} dir_stats_t;

/**
 * Walk directory and save statistics.
 *
 * @param path Input directory.
 * @param stats Output statistics.
 * @return 0 or negative error.
 */
int walk_dir(const char *path, dir_stats_t *stats);

#endif
