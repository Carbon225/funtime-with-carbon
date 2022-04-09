#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

static const char HELP[] =
        "SO Lab5 Zad3 - Jakub Karbowski\n"
        "Usage:\n"
        "%s FIFO OUTPUT N\n"
        "  FIFO   - path to fifo\n"
        "  OUTPUT - path to output file\n"
        "  N      - read burst size\n";

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    const char *fifo_path = argv[1];
    const char *output_file_path = argv[2];

    char *endptr;
    int burst_size = (int) strtol(argv[3], &endptr, 10);
    if (*endptr || burst_size < 0)
    {
        fprintf(stderr, "Invalid N: %s\n", argv[3]);
        return -1;
    }

    int fifo_fd = open(fifo_path, O_RDONLY);
    if (fifo_fd < 0)
    {
        fprintf(stderr, "Could not open fifo\n");
        return -1;
    }

    int output_fd = open(output_file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd < 0)
    {
        fprintf(stderr, "Could not open output file\n");
        close(fifo_fd);
        return -1;
    }

    close(fifo_fd);
    close(output_fd);

    return 0;
}
