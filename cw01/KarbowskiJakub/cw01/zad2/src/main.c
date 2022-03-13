#include <stdio.h>
#include "cli.h"
#include "zad1_lib.h"

int main(int argc, char **argv)
{
    int err;

#ifdef ZAD1_LIB_DLL
    err = zad1_lib_load();
    if (err)
    {
        fprintf(stderr, "Could not load dll\n");
        return -1;
    };
#endif

    err = cli_process_commands(argc, argv);
    return err;
}
