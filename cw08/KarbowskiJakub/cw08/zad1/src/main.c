#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pgm.h"
#include "invert_1.h"
#include "invert_2.h"

static const char HELP[] = "Usage: %s THREADS numbers|block INPUT OUTPUT\n";

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        fprintf(stderr, HELP, argv[0]);
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

    int err;

    if (!strcmp("numbers", argv[2]))
    {
        err = invert_1(&img, nthr);
    }
    else if (!strcmp("block", argv[2]))
    {
        err = invert_2(&img, nthr);
    }
    else
    {
        fprintf(stderr, HELP, argv[0]);
        err = -1;
    }

    if (!err) err = pgm_save(&img, argv[4]);

    image_free(&img);

    return err;
}
