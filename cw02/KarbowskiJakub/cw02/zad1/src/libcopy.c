#include "libcopy.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int copy_file(const char *in_path, const char *out_path)
{
    int err = 0;
    fpos_t buf_size = 256;
    char *buf = 0;
    FILE *fin = 0;
    FILE *fout = 0;

    do
    {
        buf = malloc(buf_size);
        if (!buf)
        {
            err = -1;
            break;
        }

        fin = fopen(in_path, "r");
        if (!fin)
        {
            err = -1;
            break;
        }

        fout = fopen(out_path, "w");
        if (!fout)
        {
            err = -1;
            break;
        }

        while (!feof(fin))
        {
            fpos_t line_start;
            err = fgetpos(fin, &line_start);
            if (err) break;

            int ignore = 1;

            for (;;)
            {
                int c = fgetc(fin);
                if (feof(fin)) break;
                if (ferror(fin))
                {
                    err = -1;
                    break;
                }

                if (!isspace(c)) ignore = 0;

                if (c == '\n') break;
            }
            if (err) break;

            if (ignore) continue;

            fpos_t line_end;
            err = fgetpos(fin, &line_end);
            if (err) break;

            fpos_t line_size = line_end - line_start;

            err = fsetpos(fin, &line_start);
            if (err) break;

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

            size_t n = fread(buf, 1, line_size, fin);
            if (n != line_size)
            {
                err = -1;
                break;
            }

            n = fwrite(buf, 1, line_size, fout);
            if (n != line_size)
            {
                err = -1;
                break;
            }
        }
    } while (0);

    fclose(fin);
    fclose(fout);
    free(buf);

    return err;
}
