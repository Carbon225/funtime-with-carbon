#include "zad1.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define STR_BUF_SIZE (1024)

static const char TMP_FILE_TEMPLATE[] = "/tmp/jkzad2.XXXXXX";

/**
 * Escape string with single quotes.
 * Given "text\0" as input would write "'text'\0" to output.
 *
 * @param out Output buffer.
 * @param n Size of output buffer.
 * @param in Input string.
 * @return 0 if successful, else error.
 */
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

/**
 * Load file contents to a new block.
 *
 * @param fd File descriptor.
 * @return Pointer to created block or NULL if error.
 */
static block_t* create_block_from_file(int fd)
{
    block_t *block = NULL;

    do
    {
        if (fd < 0) break;

        // get file size by seeking to EOF and back
        off_t size = lseek(fd, 0, SEEK_END);
        if (size < 0) break;
        off_t off = lseek(fd, 0, SEEK_SET);
        if (off) break;

        block = malloc(sizeof(*block));
        if (!block) break;

        block->size = size;
        block->data = malloc(size);
        if (!block->data) break;

        size_t written = read(fd, block->data, size);
        if (written != size) break;

        return block;
    } while (0);

    // cleanup
    if (block)
    {
        free(block->data);
        free(block);
    }
    return NULL;
}

barr_t* barr_alloc(size_t size)
{
    barr_t *barr = NULL;

    do
    {
        barr = malloc(sizeof(*barr));
        if (!barr) break;

        barr->size = size;
        barr->blocks = calloc(size, sizeof(*barr->blocks));
        if (!barr->blocks) break;

        return barr;
    } while (0);

    // cleanup
    if (barr)
    {
        free(barr->blocks);
        free(barr);
    }
    return NULL;
}

void barr_free(barr_t *barr)
{
    if (!barr) return;

    if (barr->blocks)
    {
        for (size_t i = 0; i < barr->size; ++i)
        {
            if (barr->blocks[i])
            {
                free(barr->blocks[i]->data);
                free(barr->blocks[i]);
            }
        }
        free(barr->blocks);
    }

    free(barr);
}

int generate_stats_file(const char *in_filename)
{
    int err;
    char cmd[STR_BUF_SIZE];
    char in_filename_quoted[STR_BUF_SIZE];
    char out_filename[sizeof(TMP_FILE_TEMPLATE)];
    int out_file_fd = -1;

    do
    {
        if (!in_filename) break;

        memcpy(out_filename, TMP_FILE_TEMPLATE, sizeof(TMP_FILE_TEMPLATE));
        out_file_fd = mkstemp(out_filename);
        if (out_file_fd < 0) break;

        err = quote_string(
            in_filename_quoted,
            sizeof(in_filename_quoted),
            in_filename
        );
        if (err) break;

        int n = snprintf(
            cmd,
            sizeof(cmd),
            "wc %s > %s",
            in_filename_quoted,
            out_filename
        );
        if (n >= sizeof(cmd)) break;

        err = system(cmd);
        if (err) break;

        // file will be automatically
        // removed when closed
        unlink(out_filename);

        return out_file_fd;
    } while (0);

    // cleanup
    if (out_file_fd >= 0)
    {
        close(out_file_fd);
        unlink(out_filename);
    }
    return -1;
}

int barr_block_load(barr_t *barr, int in_file_fd, size_t *new_index)
{
    if (!barr || !barr->blocks || in_file_fd < 0 || !new_index) return -1;

    // find first free block
    size_t index;
    int found = 0;
    for (size_t i = 0; i < barr->size; ++i)
    {
        if (!barr->blocks[i])
        {
            found = 1;
            index = i;
            break;
        }
    }

    // no free blocks
    if (!found) return -1;

    barr->blocks[index] = create_block_from_file(in_file_fd);
    if (!barr->blocks[index]) return -1;

    *new_index = index;

    return 0;
}

int barr_block_delete(barr_t *barr, size_t b_index)
{
    if (!barr ||
        !barr->blocks ||
        b_index < 0 ||
        b_index >= barr->size ||
        !barr->blocks[b_index])
        return -1;

    free(barr->blocks[b_index]->data);
    free(barr->blocks[b_index]);
    barr->blocks[b_index] = NULL;

    return 0;
}
