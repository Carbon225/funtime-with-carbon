#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "calculator.h"

static const char HELP[] =
        "SO Lab3 Zad2 - Jakub Karbowski\n"
        "Usage:\n"
        "%s DX N - calculate with rectangle "
        "width DX using N processes\n";

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    char *endptr;
    double dx = strtod(argv[1], &endptr);
    if (*endptr || dx <= 0)
    {
        fprintf(stderr, "Invalid DX: %s\n", argv[1]);
        return -1;
    }
    long n = strtol(argv[2], &endptr, 10);
    if (*endptr || n <= 0 || n > INT_MAX)
    {
        fprintf(stderr, "Invalid N: %s\n", argv[2]);
        return -1;
    }

    int err = run_calculator(dx, (int) n);
    if (err) fprintf(stderr, "Error!\n");

    return err;
}
