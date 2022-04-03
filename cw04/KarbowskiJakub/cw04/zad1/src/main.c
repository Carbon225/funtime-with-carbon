#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

static const char HELP[] =
        "SO Lab4 Zad1 - Jakub Karbowski\n"
        "Usage:\n"
        "%s ignore|handler|mask|pending [exec]\n";

static void handler(int sig)
{
    printf("Got signal: %s\n", strsignal(sig));
}

int main(int argc, char **argv)
{
    int do_exec = argc == 3 && !strcmp("exec", argv[2]);
    int super_secret_mode = argc == 3 && !strcmp("SUPER_SECRET_MODE", argv[2]);

    if (argc != 2 && !super_secret_mode && !do_exec)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    if (do_exec) argc--;

    pid_t pid = -1;
    struct sigaction act = {0};
    sigset_t set;

    if (!strcmp("ignore", argv[1]))
    {
        if (!super_secret_mode)
        {
            act.sa_handler = SIG_IGN;
            sigaction(SIGUSR1, &act, NULL);

            sigaction(SIGUSR1, NULL, &act);
            printf("[Parent] Is%s ignored\n",
                   act.sa_handler == SIG_IGN ? "" : " not"
            );

            raise(SIGUSR1);

            if (do_exec)
            {
                char **args = malloc((argc + 2) * sizeof *args);
                memcpy(args, argv, (argc + 2) * sizeof *args);
                args[argc] = "SUPER_SECRET_MODE";
                args[argc + 1] = 0;
                execv(args[0], args);
            }
            else
            {
                pid = fork();
                if (pid) waitpid(pid, NULL, 0);
            }
        }

        if (super_secret_mode || !pid)
        {
            sigaction(SIGUSR1, NULL, &act);
            printf("[%s] Is%s ignored\n",
                   super_secret_mode ? "Exec" : "Child",
                   act.sa_handler == SIG_IGN ? "" : " not"
            );
            raise(SIGUSR1);
            exit(0);
        }
    }
    else if (!strcmp("handler", argv[1]))
    {
        if (!super_secret_mode)
        {
            act.sa_handler = handler;
            sigaction(SIGUSR1, &act, NULL);

            raise(SIGUSR1);

            if (do_exec)
            {
                char **args = malloc((argc + 2) * sizeof *args);
                memcpy(args, argv, (argc + 2) * sizeof *args);
                args[argc] = "SUPER_SECRET_MODE";
                args[argc + 1] = 0;
                execv(args[0], args);
            }
            else
            {
                pid = fork();
                if (pid) waitpid(pid, NULL, 0);
            }
        }

        if (super_secret_mode || !pid)
        {
            raise(SIGUSR1);
            exit(0);
        }
    }
    else if (!strcmp("mask", argv[1]))
    {
        if (!super_secret_mode)
        {
            sigemptyset(&set);
            sigaddset(&set, SIGUSR1);
            sigprocmask(SIG_BLOCK, &set, NULL);

            sigprocmask(0, NULL, &set);
            printf("[Parent] Is%s masked\n",
                   sigismember(&set, SIGUSR1) ? "" : " not"
            );

            raise(SIGUSR1);

            if (do_exec)
            {
                char **args = malloc((argc + 2) * sizeof *args);
                memcpy(args, argv, (argc + 2) * sizeof *args);
                args[argc] = "SUPER_SECRET_MODE";
                args[argc + 1] = 0;
                execv(args[0], args);
            }
            else
            {
                pid = fork();
                if (pid) waitpid(pid, NULL, 0);
            }
        }

        if (super_secret_mode || !pid)
        {
            sigprocmask(0, NULL, &set);
            printf("[%s] Is%s masked\n",
                super_secret_mode ? "Exec" : "Child",
                sigismember(&set, SIGUSR1) ? "" : " not"
            );
            raise(SIGUSR1);
            exit(0);
        }
    }
    else if (!strcmp("pending", argv[1]))
    {
        if (!super_secret_mode)
        {
            sigemptyset(&set);
            sigaddset(&set, SIGUSR1);
            sigprocmask(SIG_BLOCK, &set, NULL);

            raise(SIGUSR1);

            sigpending(&set);
            printf("[Parent] Is%s pending\n",
                   sigismember(&set, SIGUSR1) ? "" : " not"
            );

            if (do_exec)
            {
                char **args = malloc((argc + 2) * sizeof *args);
                memcpy(args, argv, (argc + 2) * sizeof *args);
                args[argc] = "SUPER_SECRET_MODE";
                args[argc + 1] = 0;
                execv(args[0], args);
            }
            else
            {
                pid = fork();
                if (pid) waitpid(pid, NULL, 0);
            }
        }

        if (super_secret_mode || !pid)
        {
            sigpending(&set);
            printf("[%s] Is%s pending\n",
                super_secret_mode ? "Exec" : "Child",
                sigismember(&set, SIGUSR1) ? "" : " not"
            );
            exit(0);
        }
    }
    else
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    return 0;
}
