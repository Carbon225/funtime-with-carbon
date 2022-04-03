#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void handler3(int sig, siginfo_t *info, void *ucontext)
{
    printf("Got signal: %s\n"
        "\tSignal number:\t%d\n"
        "\tSender PID:\t%lld\n",
        strsignal(info->si_signo),
        info->si_signo,
        (long long) info->si_pid
    );
}

static void handler1(int sig)
{
    printf("Got signal %d: %s\n", sig, strsignal(sig));
}

static int SIGNALS[] = {
        SIGUSR1,
        SIGHUP,
        0,
};

int main(int argc, char **argv)
{
    SIGNALS[2] = SIGRTMIN;

    struct sigaction act = {0};



    // ----------------------------------------------------------

    for (int i = 0; i < sizeof(SIGNALS) / sizeof(*SIGNALS); ++i)
    {
        int sig = SIGNALS[i];

        printf("\nTesting signal %d: %s\n", sig, strsignal(sig));

        act.sa_sigaction = handler3;
        act.sa_flags = SA_SIGINFO;
        sigaction(sig, &act, NULL);
        raise(sig);
    }

    // ----------------------------------------------------------

    if (!fork())
    {
        printf("\nTesting SA_RESETHAND\n");
        act.sa_handler = handler1;
        act.sa_flags = SA_RESETHAND;
        sigaction(SIGUSR1, &act, NULL);
        printf("Raise\n");
        raise(SIGUSR1);
        printf("Raise\n");
        raise(SIGUSR1);
        exit(0);
    }
    wait(NULL);

    // ----------------------------------------------------------

    printf("\nTesting without SA_NODEFER\n");
    act.sa_sigaction = handler3;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    {
        pid_t parent = getpid();
        if (!fork())
        {
            union sigval val;
            printf("Raise\n");
            sigqueue(parent, SIGUSR1, val);
            printf("Raise\n");
            sigqueue(parent, SIGUSR1, val);
            exit(0);
        }
        wait(NULL);
        sleep(2);
    }

    // ----------------------------------------------------------

    printf("\nTesting with SA_NODEFER\n");
    act.sa_sigaction = handler3;
    act.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGUSR1, &act, NULL);
    {
        pid_t parent = getpid();
        if (!fork())
        {
            union sigval val;
            printf("Raise\n");
            sigqueue(parent, SIGUSR1, val);
            printf("Raise\n");
            sigqueue(parent, SIGUSR1, val);
            exit(0);
        }
        wait(NULL);
        sleep(2);
    }

    // ----------------------------------------------------------

    return 0;
}
