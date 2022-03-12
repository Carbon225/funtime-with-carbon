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
 * @param in_file_fd Input file descriptor.
 * @param new_index [Output] Index of created block.
 * @return 0 or negative error.
 */
int barr_block_load(barr_t *barr, int in_file_fd, size_t *new_index);

/**
 * Delete block at index.
 *
 * @param barr Pointer to block array.
 * @param b_index Block index.
 * @return 0 if successful, else error.
 */
int barr_block_delete(barr_t *barr, size_t b_index);

/**
 * Calls wc on in_file and writes results
 * to new temporary file.
 * User MUST close created file.
 *
 * @param in_filename Input file path.
 * @return Created file descriptor or negative error.
 */
int generate_stats_file(const char *in_filename);

#endif
