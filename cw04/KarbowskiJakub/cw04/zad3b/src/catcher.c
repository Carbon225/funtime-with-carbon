#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

typedef enum send_mode_t
{
    MODE_KILL,
    MODE_SIGQUEUE,
    MODE_SIGRT,
} send_mode_t;

static volatile send_mode_t g_send_mode;
static volatile int g_num_received = 0;
static volatile bool g_got_stop = false;
static volatile pid_t g_sender_pid;

static void handler(int sig, siginfo_t *info, void *ucontext)
{
    // get sender's mode
    if (sig == SIGRTMIN + 0 || sig == SIGRTMIN + 1)
        g_send_mode = MODE_SIGRT;
    else if (info->si_code == SI_USER)
        g_send_mode = MODE_KILL;
    else if (info->si_code == SI_QUEUE)
        g_send_mode = MODE_SIGQUEUE;

    g_sender_pid = info->si_pid;

    if (sig == SIGUSR1 || sig == SIGRTMIN + 0)
        g_num_received++;
    else if (sig == SIGUSR2 || sig == SIGRTMIN + 1)
        g_got_stop = true;
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

    while (!g_got_stop) pause();

    printf("Catcher got %d signals\n", g_num_received);

    // SEND PONG

    for (int i = 0; i < g_num_received; ++i)
    {
        switch (g_send_mode)
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

    switch (g_send_mode)
    {
        case MODE_KILL:
            kill(g_sender_pid, SIGUSR2);
            break;

        case MODE_SIGQUEUE:
            sigqueue(g_sender_pid, SIGUSR2, (union sigval) g_num_received);
            break;

        case MODE_SIGRT:
            kill(g_sender_pid, SIGRTMIN + 1);
            break;
    }

    return 0;
}
