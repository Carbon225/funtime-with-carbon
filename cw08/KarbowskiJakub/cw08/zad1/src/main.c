#include <stdio.h>
#include <stdlib.h>

#include "pgm.h"
#include "invert_1.h"

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s THREADS numbers|block INPUT OUTPUT\n", argv[0]);
        return -1;
    }

    char *endptr;
    int nthr = (int) strtol(argv[1], &endptr, 10);
    if (*endptr || nthr < 1)
    {
        fprintf(stderr, "Invalid THREADS: %s\n", argv[1]);
        return -1;
    }

    image_t img;

    if (pgm_load(&img, argv[3]))
        return -1;

    if (invert_1(&img, nthr))
        return -1;

    if (pgm_save(&img, argv[4]))
        return -1;

    return 0;
}
