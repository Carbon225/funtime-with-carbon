#include <stdio.h>
#include "parser.h"
#include "executor.h"

int main(int argc, char** argv)
{
    if (argc != 2) return -1;

    const char *input_file = argv[1];

    parser_t parser;
    parser_init(&parser);

    FILE *f = fopen(input_file, "r");
    if (!f) return -1;

    int err = 0;

    while (!feof(f))
    {
        int c = fgetc(f);
        if (ferror(f))
        {
            err = -1;
            break;
        }
        if (feof(f))
            c = '\n';

        err = parser_feed(&parser, (char) c);
        if (err) break;
    }

    fclose(f);

    if (parser.state == PARSER_S_ERR)
        fprintf(stderr, "%s\n", parser.err_msg);
    else
    {
        err = program_execute(&parser.program);
    }

    return err;
}
