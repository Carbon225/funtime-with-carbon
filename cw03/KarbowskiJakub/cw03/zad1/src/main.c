#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static const char HELP[] =
        "SO Lab3 Zad1 - Jakub Karbowski\n"
        "Usage:\n"
        "%s N - start N processes\n";

static void child_task()
{
    printf("Hello from %lld\n", (long long) getpid());
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, HELP, argv[0]);
        return -1;
    }

    char *endptr;
    long n = strtol(argv[1], &endptr, 10);
    if (*endptr || n < 0)
    {
        fprintf(stderr, "Invalid N: %s\n", argv[1]);
        return -1;
    }

    printf("Spawning %ld processes\n", n);
    for (long i = 0; i < n; ++i)
    {
        if (!fork()) child_task();
    }
    while (wait(NULL) >= 0);

    return 0;
}
