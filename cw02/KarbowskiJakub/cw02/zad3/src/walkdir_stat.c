#include "walkdir.h"

#ifdef IMPL_STAT

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

int process_dir(const char *root_path, dir_stats_t *stats);

static int process_path(const char *path, dir_stats_t *stats)
{
    if (!path || !stats) return -1;

    int err;
    struct stat stat;

    err = lstat(path, &stat);
    if (err) return -1;

    char atime[26];
    if (ctime_r(&stat.st_atime, atime) != atime)
        return -1;
    if (atime[strlen(atime) - 1] == '\n')
        atime[strlen(atime) - 1] = 0;

    char mtime[26];
    if (ctime_r(&stat.st_mtime, mtime) != mtime)
        return -1;
    if (mtime[strlen(mtime) - 1] == '\n')
        mtime[strlen(mtime) - 1] = 0;

    const char *type;

    if (S_ISFIFO(stat.st_mode))
    {
        stats->n_fifo++;
        type = "fifo";
    }
    else if (S_ISCHR(stat.st_mode))
    {
        stats->n_chr++;
        type = "chr";
    }
    else if (S_ISDIR(stat.st_mode))
    {
        err = process_dir(path, stats);
        if (err) return -1;
        stats->n_dir++;
        type = "dir";
    }
    else if (S_ISBLK(stat.st_mode))
    {
        stats->n_blk++;
        type = "blk";
    }
    else if (S_ISREG(stat.st_mode))
    {
        stats->n_reg++;
        type = "reg";
    }
    else if (S_ISLNK(stat.st_mode))
    {
        stats->n_link++;
        type = "link";
    }
    else if (S_ISSOCK(stat.st_mode))
    {
        stats->n_sock++;
        type = "sock";
    }
    else return -1;

    printf(
        "%s:\n"
        "- links: %d\n"
        "- type:  %s\n"
        "- size:  %lldB\n"
        "- atime: %s\n"
        "- mtime: %s\n",
        path,
        stat.st_nlink,
        type,
        stat.st_size,
        atime,
        mtime
    );

    return err;
}

int process_dir(const char *root_path, dir_stats_t *stats)
{
    if (!root_path || !stats) return -1;

    int err = 0;
    DIR *root_dir = opendir(root_path);
    if (!root_dir) return -1;

    for (;;)
    {
        struct dirent *ent = readdir(root_dir);
        if (!ent) break;

        // skip . and ..
        if (!strcmp(".", ent->d_name) || !strcmp("..", ent->d_name))
            continue;

        if (strlen(root_path) + strlen(ent->d_name) + 2 > PATH_MAX)
        {
            err = -1;
            break;
        }
        char ent_path[PATH_MAX];
        strcpy(ent_path, root_path);
        strcat(ent_path, "/");
        strcat(ent_path, ent->d_name);

        err = process_path(ent_path, stats);
        if (err) break;
    }

    closedir(root_dir);

    return err;
}

int walk_dir(const char *path, dir_stats_t *stats)
{
    if (!path || !stats) return -1;

    memset(stats, 0, sizeof *stats);

    char abs_path[PATH_MAX];
    if (realpath(path, abs_path) != abs_path)
        return -1;

    return process_path(abs_path, stats);
}

#endif // IMPL_STAT
