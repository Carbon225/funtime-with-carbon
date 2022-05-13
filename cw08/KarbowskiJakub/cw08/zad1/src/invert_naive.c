#include "invert_naive.h"

int invert_naive(image_t *img)
{
    if (!img) return -1;

    for (int i = 0; i < img->w * img->h; ++i)
    {
        img->data[i] = img->max - img->data[i];
    }

    return 0;
}
