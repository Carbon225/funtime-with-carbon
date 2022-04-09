#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

static const char HELP[] =
        "SO Lab5 Zad3 - Jakub Karbowski\n"
        "Usage:\n"
        "%s FIFO LINE INPUT N\n"
        "  FIFO  - path to fifo\n"
        "  LINE  - line number\n"
        "  INPUT - path to input file\n"
        "  N     - read burst size\n";

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    const char *fifo_path = argv[1];
    const char *input_file_path = argv[3];

    char *endptr;
    int line = (int) strtol(argv[2], &endptr, 10);
    if (*endptr || line < 0)
    {
        fprintf(stderr, "Invalid LINE: %s\n", argv[2]);
        return -1;
    }

    int burst_size = (int) strtol(argv[4], &endptr, 10);
    if (*endptr || burst_size < 0)
    {
        fprintf(stderr, "Invalid N: %s\n", argv[4]);
        return -1;
    }

    int fifo_fd = open(fifo_path, O_WRONLY);
    if (fifo_fd < 0)
    {
        fprintf(stderr, "Could not open fifo\n");
        return -1;
    }

    int input_fd = open(input_file_path, O_RDONLY);
    if (input_fd < 0)
    {
        fprintf(stderr, "Could not open input file\n");
        close(fifo_fd);
        return -1;
    }

    close(fifo_fd);
    close(input_fd);

    return 0;
}
