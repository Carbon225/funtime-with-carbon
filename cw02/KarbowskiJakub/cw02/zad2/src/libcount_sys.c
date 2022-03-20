#include "libcount.h"

#ifdef IMPL_SYS

#include <unistd.h>
#include <fcntl.h>

int count_chars(const char *file, char c, char_stats_t *stats)
{
    if (!file || !stats) return -1;

    int err = 0;

    stats->n_chars = 0;
    stats->n_lines = 0;

    int f = open(file, O_RDONLY);
    if (f < 0) return -1;

    int found_in_line = 0;

    for (;;)
    {
        char cur;
        ssize_t n = read(f, &cur, 1);
        if (n == 0) break;
        if (n != 1)
        {
            err = -1;
            break;
        }

        if (cur == c)
        {
            stats->n_chars++;
            if (!found_in_line)
            {
                stats->n_lines++;
                found_in_line = 1;
            }
        }

        if (cur == '\n')
            found_in_line = 0;
    }

    close(f);
    return err;
}

#endif // IMPL_LIB
