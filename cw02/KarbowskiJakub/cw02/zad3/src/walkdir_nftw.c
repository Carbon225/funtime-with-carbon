#include "walkdir.h"

#ifdef IMPL_NFTW

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <ftw.h>
#include <time.h>

#define NOPENFD (20)

static dir_stats_t *GLOBAL_STATS;

static int nftw_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    char atime[26];
    if (ctime_r(&sb->st_atime, atime) != atime)
        return -1;

    char mtime[26];
    if (ctime_r(&sb->st_mtime, mtime) != mtime)
        return -1;

    // remove \n
    atime[24] = 0;
    mtime[24] = 0;

    const char *type;

    if (S_ISFIFO(sb->st_mode))
    {
        GLOBAL_STATS->n_fifo++;
        type = "fifo";
    }
    else if (S_ISCHR(sb->st_mode))
    {
        GLOBAL_STATS->n_chr++;
        type = "chr";
    }
    else if (S_ISDIR(sb->st_mode))
    {
        GLOBAL_STATS->n_dir++;
        type = "dir";
    }
    else if (S_ISBLK(sb->st_mode))
    {
        GLOBAL_STATS->n_blk++;
        type = "blk";
    }
    else if (S_ISREG(sb->st_mode))
    {
        GLOBAL_STATS->n_reg++;
        type = "reg";
    }
    else if (S_ISLNK(sb->st_mode))
    {
        GLOBAL_STATS->n_link++;
        type = "link";
    }
    else if (S_ISSOCK(sb->st_mode))
    {
        GLOBAL_STATS->n_sock++;
        type = "sock";
    }
    else return -1;

    printf(
        "%s:\n"
        "- links: %lld\n"
        "- type:  %s\n"
        "- size:  %lldB\n"
        "- atime: %s\n"
        "- mtime: %s\n",
        fpath,
        (long long) sb->st_nlink,
        type,
        (long long) sb->st_size,
        atime,
        mtime
    );

    return 0;
}

int walk_dir(const char *path, dir_stats_t *stats)
{
    if (!path || !stats) return -1;

    char abs_path[PATH_MAX];
    if (realpath(path, abs_path) != abs_path)
        return -1;

    memset(stats, 0, sizeof *stats);
    GLOBAL_STATS = stats;

    return nftw(abs_path, nftw_callback, NOPENFD, FTW_PHYS);
}

#endif // IMPL_NFTW
