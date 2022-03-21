#include "walkdir.h"

#ifdef IMPL_STAT

#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/param.h>

/**
 * Process all entries in directory.
 * Upon return, root_fd will be closed.
 *
 * @param root_fd Directory fd.
 * @param stats Output stats.
 * @return 0 or negative error.
 */
int process_dir(int root_fd, dir_stats_t *stats);

/**
 * Process this file or directory.
 * Upon return, root_fd will be closed.
 *
 * @param root_fd Input fd.
 * @param stats Output stats.
 * @return 0 or negative error.
 */
int process_path(int root_fd, dir_stats_t *stats);

int process_dir(int root_fd, dir_stats_t *stats)
{
    int err = 0;
    DIR *root_dir = NULL;

    do
    {
        if (root_fd < 0 || !stats)
        {
            err = -1;
            break;
        }

        root_dir = fdopendir(root_fd);
        if (!root_dir)
        {
            err = -1;
            break;
        }
        // taken over by root_dir
        root_fd = -1;

        for (;;)
        {
            struct dirent *ent = readdir(root_dir);
            if (!ent) break;

            if (!strcmp(".", ent->d_name) || !strcmp("..", ent->d_name))
                continue;

            int ent_fd = openat(dirfd(root_dir), ent->d_name, O_RDONLY);
            if (ent_fd < 0)
            {
                err = -1;
                break;
            }

            err = process_path(ent_fd, stats);
            // ent_fd is closed
            if (err) break;
        }
    } while (0);

    if (root_dir)
        closedir(root_dir);
    else if (root_fd >= 0)
        close(root_fd);

    return err;
}

int process_path(int root_fd, dir_stats_t *stats)
{
    int err = 0;

    do
    {
        if (root_fd < 0 || !stats)
        {
            err = -1;
            break;
        }

        struct stat stat;
        err = fstat(root_fd, &stat);
        if (err) break;

        char path[MAXPATHLEN];
        if (fcntl(root_fd, F_GETPATH, path) == -1)
        {
            err = -1;
            break;
        }

        char atime[26];
        char mtime[26];
        char* ret = ctime_r(&stat.st_atimespec.tv_sec, atime);
        if (!ret)
        {
            err = -1;
            break;
        }
        ret = ctime_r(&stat.st_mtimespec.tv_sec, mtime);
        if (!ret)
        {
            err = -1;
            break;
        }

        atime[24] = 0;
        mtime[24] = 0;

        const char *type;

        if (stat.st_mode & S_IFIFO)
        {
            stats->n_fifo++;
            type = "fifo";
        }
        else if (stat.st_mode & S_IFCHR)
        {
            stats->n_chr++;
            type = "chr";
        }
        else if (stat.st_mode & S_IFDIR)
        {
            err = process_dir(root_fd, stats);
            // root_fd is closed
            root_fd = -1;
            if (err) break;
            stats->n_dir++;
            type = "dir";
        }
        else if (stat.st_mode & S_IFBLK)
        {
            stats->n_blk++;
            type = "blk";
        }
        else if (stat.st_mode & S_IFREG)
        {
            stats->n_reg++;
            type = "reg";
        }
        else if (stat.st_mode & S_IFLNK)
        {
            stats->n_link++;
            type = "link";
        }
        else if (stat.st_mode & S_IFSOCK)
        {
            stats->n_sock++;
            type = "sock";
        }
        else
        {
            err = -1;
            break;
        }

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
    } while (0);

    if (root_fd >= 0)
        close(root_fd);

    return err;
}

int walk_dir(const char *path, dir_stats_t *stats)
{
    if (!path || !stats) return -1;

    memset(stats, 0, sizeof *stats);

    int fd = open(path, O_RDONLY | O_DIRECTORY);
    if (fd < 0) return -1;

    // fd will be closed
    return process_path(fd, stats);
}

#endif // IMPL_STAT
