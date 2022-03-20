#include "libcount.h"

#ifdef IMPL_LIB

#include <stdio.h>

int count_chars(const char *file, char c, char_stats_t *stats)
{
    if (!file || !stats) return -1;

    int err = 0;

    stats->n_chars = 0;
    stats->n_lines = 0;

    FILE *f = fopen(file, "r");
    if (!f) return -1;

    int found_in_line = 0;

    for (;;)
    {
        int cur = fgetc(f);
        if (feof(f)) break;
        if (ferror(f))
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

    fclose(f);
    return err;
}

#endif // IMPL_LIB
