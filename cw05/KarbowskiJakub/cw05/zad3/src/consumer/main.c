#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int receive(int output_fd, int fifo_fd, int burst_size);

int append_to_line(int fd, int line, const void *buf, int n);

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

    int err = receive(output_fd, fifo_fd, burst_size);

    close(fifo_fd);
    close(output_fd);

    if (err) fprintf(stderr, "Error\n");

    return 0;
}

int receive(int output_fd, int fifo_fd, int burst_size)
{
    uint8_t *buf = malloc(sizeof(int) + burst_size);
    uint8_t *data = buf + sizeof(int);

    int err = 0;

    for (;;)
    {
        int n = (int) read(fifo_fd, buf, sizeof(int) + burst_size);
        if (n <= 0) break;

        if (n < sizeof(int))
        {
            err = -1;
            break;
        }

        err = append_to_line(output_fd, *(int*)buf, data, n - sizeof(int));
        if (err) break;
    }

    free(buf);

    return err;
}

int append_to_line(int fd, int line, const void *buf, int n)
{
    FILE *tmp = tmpfile();
    if (!tmp) return -1;

    lseek(fd, 0, SEEK_SET);
    int cur_line = 0;

    for (;;)
    {
        char c;
        if (read(fd, &c, 1) != 1)
        {
            if (cur_line <= line)
            {
                c = '\n';
            }
            else
            {
                break;
            }
        }

        if (c == '\n')
        {
            if (cur_line == line)
            {
                fwrite(buf, 1, n, tmp);
            }

            cur_line++;
        }

        fputc(c, tmp);
    }

    fseek(tmp, 0, SEEK_SET);
    lseek(fd, 0, SEEK_SET);

    for (;;)
    {
        int c = fgetc(tmp);
        if (c < 0) break;

        printf("%c\n", c);
        write(fd, &c, 1);
    }

    fclose(tmp);

    return 0;
}
