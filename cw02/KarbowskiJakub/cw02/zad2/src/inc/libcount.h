#ifndef JK_02_02_LIBCOUNT_H
#define JK_02_02_LIBCOUNT_H

#if !defined(IMPL_LIB) && !defined(IMPL_SYS)
#define IMPL_LIB
#endif

typedef struct char_stats_t
{
    int n_chars;
    int n_lines;
} char_stats_t;

/**
 * Count occurrences of char c
 * and lines containing c.
 *
 * @param file Input file path.
 * @param stats Output stats struct.
 * @return 0 or negative error.
 */
int count_chars(const char *file, char c, char_stats_t *stats);

#endif
