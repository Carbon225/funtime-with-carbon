#ifndef JK_ZAD1_INC_ZAD1_H
#define JK_ZAD1_INC_ZAD1_H

#include <stddef.h>

typedef struct block_t
{
    size_t size;
    void *data;
} block_t;

typedef struct barr_t
{
    size_t size;
    block_t **blocks;
} barr_t;

/**
 * Allocate an empty block array.
 *
 * @param size Number of blocks.
 * @return Pointer to block array or NULL if error.
 */
barr_t* barr_alloc(size_t size);

/**
 * Delete a block array.
 * Frees memory pointed to by barr.
 *
 * @param barr Pointer to block array.
 */
void barr_free(barr_t *barr);

/**
 * Load file contents into a free block.
 *
 * @param barr Pointer to block array.
 * @param in_file File path.
 * @return Index of new block or negative error.
 */
int barr_block_load(barr_t *barr, const char *in_file);

/**
 * Delete block at index.
 *
 * @param barr Pointer to block array.
 * @param b_index Block index.
 * @return 0 if successful, else error.
 */
int barr_block_delete(barr_t *barr, int b_index);

/**
 * Calls wc 'in_file' > 'out_file'.
 *
 * @param in_file Input file path.
 * @param out_file Output file path.
 * @return 0 if success, else error.
 */
int generate_stats_file(const char *in_file, const char *out_file);

#endif
