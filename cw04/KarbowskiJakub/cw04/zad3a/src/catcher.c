#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

typedef enum send_mode_t
{
    MODE_KILL,
    MODE_SIGQUEUE,
    MODE_SIGRT,
} send_mode_t;

static volatile int g_usr1_count = 0;
static volatile int g_usr2_count = 0;
static volatile send_mode_t g_sender_mode;
static volatile pid_t g_sender_pid;

static void handler(int sig, siginfo_t *info, void *ucontext)
{
    // get sender's mode
    if (sig == SIGRTMIN + 0 || sig == SIGRTMIN + 1)
        g_sender_mode = MODE_SIGRT;
    else if (info->si_code == SI_USER)
        g_sender_mode = MODE_KILL;
    else if (info->si_code == SI_QUEUE)
        g_sender_mode = MODE_SIGQUEUE;

    g_sender_pid = info->si_pid;

    if (sig == SIGUSR1 || sig == SIGRTMIN + 0)
        g_usr1_count++;
    else if (sig == SIGUSR2 || sig == SIGRTMIN + 1)
        g_usr2_count++;
}

int main(int argc, char **argv)
{
    printf("PID: %lld\n", (long long) getpid());

    // SIGNAL SETUP

    struct sigaction act = {0};
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGRTMIN + 0, &act, NULL);
    sigaction(SIGRTMIN + 1, &act, NULL);

    // WAIT FOR PING

    sigset_t set, oldset;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);
    sigaddset(&set, SIGRTMIN+1);
    // block USR2 to safely access g_usr2_count
    sigprocmask(SIG_BLOCK, &set, &oldset);
    // -- critical section start
    while (!g_usr2_count)
    {
        // -- critical section end
        // restore old mask and wait for a signal
        sigsuspend(&oldset);
        // handler was executed
        // USR2 is blocked again
        // -- critical section start
    }
    // -- critical section end
    // restore original mask
    sigprocmask(SIG_SETMASK, &oldset, NULL);

    printf("Catcher got %d signals\n", g_usr1_count);

    // SEND PONG

    for (int i = 0; i < g_usr1_count; ++i)
    {
        switch (g_sender_mode)
        {
            case MODE_KILL:
                kill(g_sender_pid, SIGUSR1);
                break;

            case MODE_SIGQUEUE:
                sigqueue(g_sender_pid, SIGUSR1, (union sigval) 0);
                break;

            case MODE_SIGRT:
                kill(g_sender_pid, SIGRTMIN + 0);
                break;
        }
    }

    // SEND STOP

    switch (g_sender_mode)
    {
        case MODE_KILL:
            kill(g_sender_pid, SIGUSR2);
            break;

        case MODE_SIGQUEUE:
            sigqueue(g_sender_pid, SIGUSR2, (union sigval) g_usr1_count);
            break;

        case MODE_SIGRT:
            kill(g_sender_pid, SIGRTMIN + 1);
            break;
    }

    return 0;
}
