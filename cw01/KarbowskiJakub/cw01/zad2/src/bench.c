#include "bench.h"

#include <stdio.h>
#include <unistd.h>
#include "zad1.h"
#include "time_utils.h"

#define BENCH1_COUNT (1 << 9)
#define BENCH2_COUNT (1 << 15)
#define BENCH4_COUNT (1 << 12)
#define MAX_FILE_COUNT (1 << 8)
#define MAX_BLOCKS (1 << 15)

int benchmarks_run(int num_files, char **in_files)
{
    /*
     * This function does a lot of error checking
     * in addition to the benchmarks.
     * It obviously has an impact on runtimes,
     * but it is the only way to keep the code safe.
     */

    if (num_files > MAX_FILE_COUNT || !in_files) return -1;

    int err = 0;
    bench_t bench;
    barr_t *barr;
    // buffer for created file descriptors
    int created_fds[MAX_FILE_COUNT];
    int created_files = 0;
    // buffer for created block ids
    size_t created_block_idx[MAX_BLOCKS];
    int created_blocks = 0;

    do
    {
        barr = barr_alloc(MAX_BLOCKS + 1);
        if (!barr) break;

        // BENCH WC_FILES

        bench_start(&bench);

        for (int count = 0; count < BENCH1_COUNT; ++count)
        {
            int fd = generate_stats_file(in_files[count % num_files]);
            if (fd < 0)
            {
                err = -1;
                break;
            }
            // keep track of MAX_FILE_COUNT files
            if (created_files < MAX_FILE_COUNT)
            {
                created_fds[created_files] = fd;
                created_files++;
            }
            else
            {
                // if buffer full, discard file
                close(fd);
            }
        }
        if (err) break;

        bench_stop(&bench);
        printf("[BENCH] WC FILES:\n");
        bench_print(&bench);

        // BENCH LOAD BLOCKS

        bench_start(&bench);

        for (int count = 0; count < BENCH2_COUNT; ++count)
        {
            size_t b_index;
            err = barr_block_load(barr, created_fds[count % created_files], &b_index);
            if (err) break;
            // keep track of MAX_BLOCKS blocks
            if (created_blocks < MAX_BLOCKS)
            {
                created_block_idx[created_blocks] = b_index;
                created_blocks++;
            }
            else
            {
                // if buffer full, delete block
                // (would mem leak otherwise)
                err = barr_block_delete(barr, b_index);
                if (err) break;
            }
        }
        if (err) break;

        bench_stop(&bench);
        printf("[BENCH] LOAD BLOCKS:\n");
        bench_print(&bench);

        // BENCH DELETE BLOCKS

        bench_start(&bench);

        // delete all blocks in buffer
        for (int i = 0; i < created_blocks; ++i)
        {
            err = barr_block_delete(barr, created_block_idx[i]);
            if (err) break;
        }
        if (err) break;
        created_blocks = 0;

        bench_stop(&bench);
        printf("[BENCH] DELETE BLOCKS:\n");
        bench_print(&bench);

        // BENCH LOAD DELETE

        bench_start(&bench);

        for (int count = 0; count < BENCH4_COUNT; ++count)
        {
            // create & remove in batches
            for (int i = 0; i < created_files; ++i)
            {
                size_t index;
                err = barr_block_load(barr, created_fds[i], &index);
                if (err) break;
                if (created_blocks < MAX_BLOCKS)
                {
                    created_block_idx[created_blocks] = index;
                    created_blocks++;
                }
                else
                {
                    err = barr_block_delete(barr, index);
                    if (err) break;
                }
            }
            if (err) break;
            for (int i = 0; i < created_blocks; ++i)
            {
                err = barr_block_delete(barr, created_block_idx[i]);
                if (err) break;
            }
            if (err) break;
            created_blocks = 0;
        }
        if (err) break;

        bench_stop(&bench);
        printf("[BENCH] LOAD DELETE:\n");
        bench_print(&bench);
    } while (0);

    // cleanup

    for (int i = 0; i < created_files; ++i)
    {
        close(created_fds[i]);
    }

    if (barr) barr_free(barr);

    return err;
}
