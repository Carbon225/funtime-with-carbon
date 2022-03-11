#include "zad1.h"

#include <stdlib.h>
#include <stdio.h>

#define STR_BUF_SIZE (1024)

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
    for (int i = 0; in[i]; ++i)
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
    for (int i = 0; in[i]; ++i)
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
 * @param path File path.
 * @return Pointer to created block or NULL if error.
 */
static block_t* create_block_from_file(const char *path)
{
    if (!path) return NULL;

    int err;
    block_t *block = NULL;

    FILE *file = fopen(path, "r");
    if (!file) goto error;

    // get file size by seeking to EOF and back
    err = fseek(file, 0, SEEK_END);
    if (err) goto error;
    fpos_t size;
    err = fgetpos(file, &size);
    if (err) goto error;
    err = fseek(file, 0, SEEK_SET);
    if (err) goto error;

    block = malloc(sizeof(*block));
    if (!block) goto error;

    block->size = size;
    block->data = malloc(size);
    if (!block->data) goto error;

    size_t written = fread(block->data, 1, size, file);
    if (written != size) goto error;

    fclose(file);
    return block;

    error:
    if (block)
    {
        free(block->data);
        free(block);
    }
    if (file) fclose(file);
    return NULL;
}

barr_t* barr_alloc(size_t size)
{
    barr_t *barr = malloc(sizeof(*barr));
    if (!barr) goto error;

    barr->size = size;
    barr->blocks = calloc(size, sizeof(*barr->blocks));
    if (!barr->blocks) goto error;

    return barr;

    error:
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
        for (int i = 0; i < barr->size; ++i)
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

int generate_stats_file(const char *in_file, const char *out_file)
{
    if (!in_file || !out_file) return -1;

    int err;
    char cmd[STR_BUF_SIZE];
    char in_file_quoted[STR_BUF_SIZE];
    char out_file_quoted[STR_BUF_SIZE];

    err = quote_string(in_file_quoted, sizeof(in_file_quoted), in_file);
    if (err) return -1;

    err = quote_string(out_file_quoted, sizeof(out_file_quoted), out_file);
    if (err) return -1;

    int n = snprintf(cmd, sizeof(cmd), "wc %s > %s", in_file_quoted, out_file_quoted);
    if (n >= sizeof(cmd)) return -1;

    return system(cmd);
}

int barr_block_load(barr_t *barr, const char *in_file)
{
    if (!barr || !barr->blocks || !in_file) return -1;

    // find first free block
    int index = -1;
    for (int i = 0; i < barr->size; ++i)
    {
        if (!barr->blocks[i])
        {
            index = i;
            break;
        }
    }

    // no free blocks
    if (index < 0) return -1;

    barr->blocks[index] = create_block_from_file(in_file);
    if (!barr->blocks[index]) return -1;

    return index;
}

int barr_block_delete(barr_t *barr, int b_index)
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
