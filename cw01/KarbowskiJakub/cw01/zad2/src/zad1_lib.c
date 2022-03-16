#include "zad1_lib.h"

#ifdef ZAD1_LIB_DLL

#include <dlfcn.h>
#include "zad1.h"

static struct
{
    barr_t* (*barr_alloc)(size_t);

    void (*barr_free)(barr_t*);

    int (*barr_block_load)(barr_t*, int, size_t*);

    int (*barr_block_delete)(barr_t*, size_t);

    int (*generate_stats_file)(const char*);
} ZAD1_LIB_FUNCTIONS;

static int ZAD1_LIB_INIT = 0;

int zad1_lib_load()
{
    if (ZAD1_LIB_INIT) return 0;

    // try loading by name
    void *handle = dlopen(ZAD1_LIB_FILE, RTLD_LAZY);
    if (!handle)
    {
        // else try loading by hardcoded absolute path
        handle = dlopen(ZAD1_LIB_PATH, RTLD_LAZY);
        if (!handle) return -1;
    }

    ZAD1_LIB_FUNCTIONS.barr_alloc = dlsym(handle, "barr_alloc");
    ZAD1_LIB_FUNCTIONS.barr_free = dlsym(handle, "barr_free");
    ZAD1_LIB_FUNCTIONS.barr_block_load = dlsym(handle, "barr_block_load");
    ZAD1_LIB_FUNCTIONS.barr_block_delete = dlsym(handle, "barr_block_delete");
    ZAD1_LIB_FUNCTIONS.generate_stats_file = dlsym(handle, "generate_stats_file");

    ZAD1_LIB_INIT = 1;

    return 0;
}

barr_t* barr_alloc(size_t size)
{
    return ZAD1_LIB_FUNCTIONS.barr_alloc(size);
}

void barr_free(barr_t *barr)
{
    ZAD1_LIB_FUNCTIONS.barr_free(barr);
}

int barr_block_load(barr_t *barr, int in_file_fd, size_t *new_index)
{
    return ZAD1_LIB_FUNCTIONS.barr_block_load(barr, in_file_fd, new_index);
}

int barr_block_delete(barr_t *barr, size_t b_index)
{
    return ZAD1_LIB_FUNCTIONS.barr_block_delete(barr, b_index);
}

int generate_stats_file(const char *in_filename)
{
    return ZAD1_LIB_FUNCTIONS.generate_stats_file(in_filename);
}

#endif
