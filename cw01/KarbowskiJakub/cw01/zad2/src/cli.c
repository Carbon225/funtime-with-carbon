#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "zad1.h"
#include "cmd.h"
#include "bench.h"

static const char CLI_BENCH_CMD[] = "bench";

static const char CLI_HELP[] =
    "SO Lab1 - Jakub Karbowski\n"
    "\nUsage:\n"
    "%s bench         - run benchmarks\n"
    "%s [COMMANDS...] - process given commands\n"
    "\nCommands:\n"
    "create_table SIZE  - create block array with SIZE empty blocks\n"
    "wc_files FILES...  - use wc on FILES and save results to first empty block\n"
    "remove_block INDEX - remove block from array at INDEX\n"
    "\nExample:\n"
    "%s create_table 10 wc_files file1.txt file2.txt remove_block 0\n"
    "\nNotes:\n"
    "When parsing filenames, command names take priority.\n"
    "For example, \"wc_files remove_block\"\n"
    "would not process file called \"remove_block\"\n"
    "but instead try executing remove_block command.\n"
    "The above example would fail.\n";

typedef struct cli_state_t
{
    barr_t *barr;
    int argc;
    char **argv;
    int current_arg;
} cli_state_t;

static int process_current_command(cli_state_t *cli)
{
    switch (cmd_parse(cli->argv[cli->current_arg]))
    {
        case CMD_CREATE_TABLE:
        {
            // advance to SIZE argument
            cli->current_arg++;

            if (cli->current_arg >= cli->argc)
            {
                fprintf(stderr, "Missing size argument\n");
                return -1;
            }

            char *endptr;
            long size = strtol(cli->argv[cli->current_arg], &endptr, 10);
            if (!*cli->argv[cli->current_arg] || *endptr || size < 1)
            {
                fprintf(stderr, "Invalid size: %s\n", cli->argv[cli->current_arg]);
                return -1;
            }

            barr_free(cli->barr);
            cli->barr = barr_alloc(size);
            if (!cli->barr)
            {
                fprintf(stderr, "Could not allocate block array\n");
                return -1;
            }

            fprintf(stderr, "Created block array with %ld blocks\n", size);
            cli->current_arg++;
            break;
        }

        case CMD_WC_FILES:
        {
            if (!cli->barr)
            {
                fprintf(stderr, "Block array not created! Use create_table first\n");
                return -1;
            }

            // advance to first file
            cli->current_arg++;

            // parse all filenames
            for (; cli->current_arg < cli->argc && cmd_parse(cli->argv[cli->current_arg]) == CMD_INVALID; cli->current_arg++)
            {
                const char *in_file = cli->argv[cli->current_arg];

                // opens tmp file
                int out_file_fd = generate_stats_file(in_file);
                if (out_file_fd < 0)
                {
                    fprintf(stderr, "Could not wc %s\n", in_file);
                    return -1;
                }

                fprintf(stderr, "Processed %s\n", in_file);

                size_t index;
                int err = barr_block_load(cli->barr, out_file_fd, &index);
                // closes tmp file
                close(out_file_fd);
                if (err)
                {
                    fprintf(stderr, "Could not load results into block array\n");
                    return -1;
                }

                size_t size = cli->barr->blocks[index]->size;

                fprintf(stderr, "Loaded results (%luB) into block %lu\n", size, index);
            }

            break;
        }

        case CMD_REMOVE_BLOCK:
        {
            if (!cli->barr)
            {
                fprintf(stderr, "Block array not created! Use create_table first\n");
                return -1;
            }

            // advance to INDEX argument
            cli->current_arg++;

            if (cli->current_arg >= cli->argc)
            {
                fprintf(stderr, "Missing index argument\n");
                return -1;
            }

            char *endptr;
            long index = strtol(cli->argv[cli->current_arg], &endptr, 10);
            if (!*cli->argv[cli->current_arg] || *endptr || index < 0)
            {
                fprintf(stderr, "Invalid index: %s\n", cli->argv[cli->current_arg]);
                return -1;
            }

            int err = barr_block_delete(cli->barr, index);
            if (err)
            {
                fprintf(stderr, "Error deleting block %ld\n", index);
                return -1;
            }

            fprintf(stderr, "Removed block %ld\n", index);
            cli->current_arg++;
            break;
        }

        default:
            fprintf(stderr, "Invalid command: %s\n", cli->argv[cli->current_arg]);
            return -1;
    }

    return 0;
}

int cli_process_commands(int argc, char **argv)
{
    if (argc < 1)
    {
        fprintf(stderr, "[FATAL] argc < 1\n");
        return -1;
    }
    else if (argc < 2)
    {
        fprintf(stderr, CLI_HELP, argv[0], argv[0], argv[0]);
        return -1;
    }

    if (!strncmp(argv[1], CLI_BENCH_CMD, sizeof(CLI_BENCH_CMD)))
    {
        return benchmarks_run(argc - 2, argv + 2);
    }

    int err;
    cli_state_t cli;
    cli.barr = NULL;
    cli.argc = argc;
    cli.argv = argv;
    cli.current_arg = 1;

    while (cli.current_arg < cli.argc)
    {
        err = process_current_command(&cli);
        if (err) break;
    }

    if (err)
    {
        fprintf(stderr, "Error processing commands\n");
        return -1;
    }

    fprintf(stderr, "Done\n");

    return 0;
}
