#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int send_data(int input_fd, int fifo_fd, int burst_size, int line);

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

    int err = send_data(input_fd, fifo_fd, burst_size, line);

    close(fifo_fd);
    close(input_fd);

    if (err) fprintf(stderr, "Error\n");

    return err;
}

int send_data(int input_fd, int fifo_fd, int burst_size, int line)
{
    uint8_t *buf = malloc(sizeof(line) + burst_size);
    uint8_t *data = buf + sizeof(line);

    *(int*)buf = line;

    for (;;)
    {
//        usleep(((rand() % 1000) + 1000) * 1000);

        int n = (int) read(input_fd, data, burst_size);
        if (n <= 0) break;

        write(fifo_fd, buf, sizeof(line) + n);
    }

    free(buf);

    return 0;
}
