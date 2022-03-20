#include <stdio.h>

static const char HELP[] =
        "SO Lab2 Zad2 - Jakub Karbowski\n"
        "Usage:\n"
        "%s CHAR FILE - count CHARs in FILE\n";

int main(int argc, char **argv)
{
    int err = 0;

    if (argc != 3)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    // first argument has to be single char
    if (argv[1][0] == 0 || argv[1][1] != 0)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    char c = argv[1][0];
    const char *in_file = argv[2];

    printf("Counting %c in %s\n", c, in_file);

    return err;
}
