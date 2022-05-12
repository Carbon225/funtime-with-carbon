#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "pizzeria.h"

#define N_COOKS 1
#define N_JANUSZ 1

int main(int argc, char **argv)
{
    printf("master\n");

    pizzeria_create();

    for (int i = 0; i < N_COOKS; ++i)
    {
        pid_t pid = fork();
        if (!pid)
        {
            execl("build/out/cook", "cook");
        }
    }

    for (int i = 0; i < N_JANUSZ; ++i)
    {
        pid_t pid = fork();
        if (!pid)
        {
            execl("build/out/janusz", "janusz");
        }
    }

    while (wait(0) >= 0);

    pizzeria_delete();

    return 0;
}
