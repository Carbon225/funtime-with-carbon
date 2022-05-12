#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "pizzeria.h"

#define N_COOKS 2
#define N_JANUSZ 4

static pid_t cooks[N_COOKS];
static pid_t janusz[N_JANUSZ];

static void sig_handler(int sig)
{
    if (sig == SIGINT)
    {
        for (int i = 0; i < N_COOKS; ++i)
        {
            kill(cooks[i], SIGINT);
        }
        for (int i = 0; i < N_JANUSZ; ++i)
        {
            kill(janusz[i], SIGINT);
        }
    }
}

int main(int argc, char **argv)
{
    struct sigaction act = {0};
    act.sa_handler = sig_handler;
    sigaction(SIGINT, &act, 0);
    sigaction(SIGTERM, &act, 0);

    pizzeria_create();

    for (int i = 0; i < N_COOKS; ++i)
    {
        pid_t pid = fork();
        if (!pid)
        {
            execl("build/out/cook", "cook", NULL);
        }
        cooks[i] = pid;
    }

    for (int i = 0; i < N_JANUSZ; ++i)
    {
        pid_t pid = fork();
        if (!pid)
        {
            execl("build/out/janusz", "janusz", NULL);
        }
        janusz[i] = pid;
    }

    while (wait(0) >= 0);

    pizzeria_delete();

    return 0;
}
