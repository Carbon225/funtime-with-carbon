#include <stdio.h>
#include <stdlib.h>
#include "libcopy.h"

static const char HELP[] =
        "SO Lab2 Zad1 - Jakub Karbowski\n"
        "Usage:\n"
        "%s              - interactive mode\n"
        "%s INPUT OUTPUT - copy contents of INPUT to OUTPUT "
        "removing empty lines\n";

int main(int argc, char **argv)
{
    int err = 0;

    // either 0 or 2 arguments
    if (argc != 3 && argc != 1)
    {
        fprintf(stderr, HELP, argv[0], argv[0]);
        return -1;
    }

    // files in arguments
    if (argc == 3)
    {
        const char *in_file = argv[1];
        const char *out_file = argv[2];
        err = copy_file(in_file, out_file);
    }
    // files from stdin
    else
    {
        ssize_t n;
        char *in_file = NULL;
        char *out_file = NULL;
        size_t in_file_cap;
        size_t out_file_cap;

        for (;;)
        {
            printf("Enter source file: ");
            fflush(stdout);
            n = getline(&in_file, &in_file_cap, stdin);
            // name + \n
            if (n > 1) break;
            printf("Invalid input!\n");
        }
        // terminate string at \n
        in_file[n - 1] = 0;

        for (;;)
        {
            printf("Enter destination file: ");
            fflush(stdout);
            n = getline(&out_file, &out_file_cap, stdin);
            if (n > 1) break;
            printf("Invalid input!\n");
        }
        out_file[n - 1] = 0;

        printf("Copying from %s to %s...\n", in_file, out_file);
        err = copy_file(in_file, out_file);
        if (err) printf("Error!\n");
        else printf("Done\n");

        free(in_file);
        free(out_file);
    }

    return err;
}
