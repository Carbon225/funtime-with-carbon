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
 * Process contents of directory.
 * Will always close root_fd.
 *
 * @param root_fd Directory descriptor.
 * @param stats Output statistics.
 * @return 0 or negative error.
 */
static int process_dir(int root_fd, dir_stats_t *stats)
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

        // convert fd to DIR
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

            // skip . and ..
            if (!strcmp(".", ent->d_name) || !strcmp("..", ent->d_name))
                continue;

            struct stat stat;
            err = fstatat(dirfd(root_dir), ent->d_name, &stat, AT_SYMLINK_NOFOLLOW);
            if (err) break;

            char abs_path[MAXPATHLEN];
            if (fcntl(dirfd(root_dir), F_GETPATH, abs_path) == -1)
            {
                err = -1;
                break;
            }
            strcat(abs_path, "/");
            strcat(abs_path, ent->d_name);

            char atime[26];
            char mtime[26];
            char* ret = ctime_r(&stat.st_atimespec.tv_sec, atime);
            if (ret != atime)
            {
                err = -1;
                break;
            }
            ret = ctime_r(&stat.st_mtimespec.tv_sec, mtime);
            if (ret != mtime)
            {
                err = -1;
                break;
            }

            // remove \n
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
                int dir_fd = openat(dirfd(root_dir), ent->d_name, O_RDONLY | O_DIRECTORY);
                if (dir_fd < 0)
                {
                    err = -1;
                    break;
                }
                err = process_dir(dir_fd, stats);
                // dir_fd is closed
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
                abs_path,
                stat.st_nlink,
                type,
                stat.st_size,
                atime,
                mtime
            );
        }
    } while (0);

    if (root_dir)
        closedir(root_dir);
    else if (root_fd >= 0)
        close(root_fd);

    return err;
}

int walk_dir(const char *path, dir_stats_t *stats)
{
    if (!path || !stats) return -1;

    memset(stats, 0, sizeof *stats);

    int root_fd = open(path, O_RDONLY | O_DIRECTORY);
    if (root_fd < 0) return -1;

    // will close root_fd
    return process_dir(root_fd, stats);
}

#endif // IMPL_STAT
