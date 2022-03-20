#include "libcopy.h"

#ifdef IMPL_SYS

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

int copy_file(const char *in_path, const char *out_path)
{
    int err = 0;
    off_t buf_size = 256;
    char *buf = NULL;
    int fin = -1;
    int fout = -1;

    do
    {
        buf = malloc(buf_size);
        if (!buf)
        {
            err = -1;
            break;
        }

        fin = open(in_path, O_RDONLY);
        if (fin < 0)
        {
            err = -1;
            break;
        }

        fout = open(out_path, O_WRONLY | O_CREAT, 0b110110110);
        if (fout < 0)
        {
            err = -1;
            break;
        }

        int eof = 0;
        while (!eof)
        {
            // remember line start
            off_t line_start = lseek(fin, 0, SEEK_CUR);
            if (line_start < 0)
            {
                err = -1;
                break;
            }

            // should line be discarded
            int ignore = 1;

            // find end of line
            for (;;)
            {
                char c;
                ssize_t n = read(fin, &c, 1);
                if (n == 0)
                {
                    eof = 1;
                    break;
                }
                if (n < 0)
                {
                    err = -1;
                    break;
                }

                if (!isspace(c)) ignore = 0;

                if (c == '\n') break;
            }
            if (err) break;

            // skip this line
            if (ignore) continue;

            // found line end
            off_t line_end = lseek(fin, 0, SEEK_CUR);
            if (line_end < 0)
            {
                err = -1;
                break;
            }

            off_t line_size = line_end - line_start;

            // go to start
            off_t p = lseek(fin, line_start, SEEK_SET);
            if (p < 0)
            {
                err = -1;
                break;
            }

            // realloc buffer if needed
            if (line_size > buf_size)
            {
                buf_size = line_size;
                buf = realloc(buf, line_size);
                if (!buf)
                {
                    err = -1;
                    break;
                }
            }

            // read line
            ssize_t n = read(fin, buf, line_size);
            if (n != line_size)
            {
                err = -1;
                break;
            }

            // write line
            n = write(fout, buf, line_size);
            if (n != line_size)
            {
                err = -1;
                break;
            }
        }
    } while (0);

    if (fin >= 0) close(fin);
    if (fin >= 0) close(fout);
    free(buf);

    return err;
}

#endif // IMPL_SYS
