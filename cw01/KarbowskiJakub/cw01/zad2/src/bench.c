#include "bench.h"

#include <stdio.h>
#include <unistd.h>
#include "zad1.h"
#include "time_utils.h"

#define BENCH_COUNT (64)
#define MAX_FILE_COUNT (32)

int benchmarks_run(int num_files, char **in_files)
{
    if (num_files > MAX_FILE_COUNT || !in_files) return -1;

    int err = 0;
    bench_t bench;
    barr_t *barr;
    int created_fds[MAX_FILE_COUNT * BENCH_COUNT];
    int created_files = 0;

    do
    {
        barr = barr_alloc(MAX_FILE_COUNT * BENCH_COUNT);
        if (!barr) break;

        bench_start(&bench);

        for (int count = 0; count < BENCH_COUNT; ++count)
        {
            for (int i = 0; i < num_files; ++i)
            {
                int fd = generate_stats_file(in_files[i]);
                if (fd < 0)
                {
                    err = -1;
                    break;
                }
                created_fds[created_files] = fd;
                created_files++;
            }
            if (err) break;
        }
        if (err) break;

        bench_stop(&bench);
        printf("[BENCH] wc files:\n");
        bench_print(&bench);
    } while (0);

    for (int i = 0; i < created_files; ++i)
    {
        close(created_fds[i]);
    }

    if (barr) barr_free(barr);

    return 0;
}
