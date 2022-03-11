#include "cli.h"

int main(int argc, char **argv)
{
    int err = cli_process_commands(argc, argv);
    return err;
}
