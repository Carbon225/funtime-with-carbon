#include "fork_search.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
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
        return 0;

    char cmd[PATH_MAX + 64];
    sprintf(cmd, "file --brief --mime -- %s", path_quoted);

    FILE *f = popen(cmd, "r");
    if (!f) return 0;

    char mime[5];
    int err = fread(mime, 1, 4, f) == 4 ? 0 : -1;
    mime[4] = 0;

    pclose(f);

    if (err) return 0;

    return !strcmp("text", mime);
}

static int search_file(const char path[], size_t root_path_len, const char pattern[])
{
    if (!is_text_file(path)) return 0;

    FILE *f = fopen(path, "r");
    if (!f) return -1;

    int err = 0;
    char *buf = NULL;

    do
    {
        size_t pat_len = strlen(pattern);
        buf = malloc(pat_len + 1);
        if (!buf)
        {
            err = -1;
            break;
        }
        buf[pat_len] = 0;

        int line_num = 1;
        int col_num = 1;

        while (!feof(f))
        {
            long match_start = ftell(f);
            if (match_start < 0)
            {
                err = -1;
                break;
            }

            err = fread(buf, 1, pat_len, f) == pat_len ? 0 : -1;
            if (err)
            {
                if (feof(f)) err = 0;
                break;
            }

            if (!memcmp(pattern, buf, pat_len))
            {
                printf("(PID %lld) %s:%d:%d\n",
                       (long long) getpid(),
                       path + root_path_len + 1,
                       line_num,
                       col_num
                       );
            }

            err = fseek(f, match_start + 1, SEEK_SET);
            if (err) break;

            if (buf[0] == '\n')
            {
                line_num++;
                col_num = 1;
            }
            else col_num++;
        }
    } while (0);

    free(buf);
    fclose(f);

    return err;
}

int process_dir(const char path[], size_t root_path_len, const char pattern[], long depth);

static int process_path(const char path[], size_t root_path_len, const char pattern[], long depth)
{
    struct stat stat;
    if (lstat(path, &stat)) return -1;

    if (S_ISDIR(stat.st_mode) && depth > 0)
    {
        pid_t pid = fork();
        if (pid < 0) return -1;

        if (!pid)
        {
            int err = process_dir(path, root_path_len, pattern, depth - 1);
            exit(err);
        }
    }
    else
    {
        if (search_file(path, root_path_len, pattern)) return -1;
    }

    return 0;
}

int process_dir(const char path[], size_t root_path_len, const char pattern[], long depth)
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

        err = process_path(ent_path, root_path_len, pattern, depth);
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


int fork_search(const char path[], const char pattern[], long depth)
{
    if (!path || !pattern) return -1;

    int err = process_path(path, strlen(path), pattern, depth);

    int status;
    while (wait(&status) >= 0)
    {
        if (!WIFEXITED(status) || WEXITSTATUS(status))
            err = -1;
    }

    return err;
}
