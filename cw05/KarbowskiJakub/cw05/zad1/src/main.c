#include <stdio.h>
#include "parser.h"
#include "executor.h"

static const char HELP[] =
        "SO Lab5 Zad1 - Jakub Karbowski\n"
        "Usage:\n"
        "%s FILE - run commands from FILE\n";

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    const char *input_file = argv[1];

    parser_t parser;
    parser_init(&parser);

    FILE *f = fopen(input_file, "r");
    if (!f)
    {
        fprintf(stderr, "Could not open file\n");
        return -1;
    }

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
        fprintf(stderr, "Parser error:\n%s\n", parser.err_msg);
    else
    {
        err = program_execute(&parser.program);
        if (err) fprintf(stderr, "Execution error\n");
    }

    return err;
}
