#include "fork_search.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

static int quote_string(char *out, size_t n, const char *in)
{
    if (!out || !in || out == in) return -1;

    // count characters to escape
    size_t out_size = 3; // start with "''\0" (3 chars)
    for (size_t i = 0; in[i]; ++i)
    {
        switch (in[i])
        {
            case '\'':
            case '\\':
                // escape with backslash
                out_size += 2;
                break;

            default:
                out_size++;
                break;
        }
    }

    if (out_size > n) return -1;

    // write quoted string
    size_t written = 0;
    out[written++] = '\'';
    for (size_t i = 0; in[i]; ++i)
    {
        switch (in[i])
        {
            case '\'':
            case '\\':
                // escape next char
                out[written++] = '\\';

            default:
                out[written++] = in[i];
                break;
        }
    }
    out[written++] = '\'';
    out[written++] = 0;

    return 0;
}

static int is_text_file(const char path[])
{
    char path_quoted[PATH_MAX];
    if (quote_string(path_quoted, sizeof path_quoted, path))
        return -1;

    char cmd[PATH_MAX + 64];
    sprintf(cmd, "file --brief --mime -- %s", path_quoted);

    FILE *f = popen(cmd, "r");
    if (!f) return -1;

    char mime[5];
    int err = fread(mime, 1, 4, f) == 4 ? 0 : -1;
    mime[4] = 0;

    pclose(f);

    if (err) return -1;

    return !strcmp("text", mime);
}

static int search_file(const char path[], size_t root_path_len, const char pattern[])
{
    if (!is_text_file(path)) return 0;

    const char *rel_path = path + root_path_len + 1;

    printf("%s\n", rel_path);

    return 0;
}

int process_dir(const char path[], size_t root_path_len, const char pattern[]);

static int process_path(const char path[], size_t root_path_len, const char pattern[])
{
    struct stat stat;
    if (lstat(path, &stat)) return -1;

    if (S_ISDIR(stat.st_mode))
    {
        pid_t pid = fork();
        if (pid < 0) return -1;

        if (!pid)
        {
            int err = process_dir(path, root_path_len, pattern);
            exit(err);
        }
    }
    else
    {
        if (search_file(path, root_path_len, pattern)) return -1;
    }

    return 0;
}

int process_dir(const char path[], size_t root_path_len, const char pattern[])
{
    int err = 0;
    DIR *root_dir = opendir(path);
    if (!root_dir) return -1;

    for (;;)
    {
        struct dirent *ent = readdir(root_dir);
        if (!ent) break;

        // skip . and ..
        if (!strcmp(".", ent->d_name) || !strcmp("..", ent->d_name))
            continue;

        if (strlen(path) + strlen(ent->d_name) + 2 > PATH_MAX)
        {
            err = -1;
            break;
        }
        char ent_path[PATH_MAX];
        strcpy(ent_path, path);
        strcat(ent_path, "/");
        strcat(ent_path, ent->d_name);

        err = process_path(ent_path, root_path_len, pattern);
        if (err) break;
    }

    closedir(root_dir);

    int status;
    while (wait(&status) >= 0)
    {
        if (!WIFEXITED(status) || WEXITSTATUS(status))
            err = -1;
    }

    return err;
}


int fork_search(const char path[], const char pattern[])
{
    if (!path || !pattern) return -1;

    int err = process_path(path, strlen(path), pattern);

    int status;
    while (wait(&status) >= 0)
    {
        if (!WIFEXITED(status) || WEXITSTATUS(status))
            err = -1;
    }

    return err;
}
