#ifndef JK_08_01_PGM_H
#define JK_08_01_PGM_H

#include <stdint.h>

typedef struct image_t
{
    uint8_t *data;
    int w;
    int h;
    int max;
} image_t;

int pgm_load(image_t *img, const char *path);

int pgm_save(const image_t *img, const char *path);

#endif
