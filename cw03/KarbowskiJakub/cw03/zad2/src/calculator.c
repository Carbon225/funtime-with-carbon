#include "calculator.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define X_FROM (0.)
#define X_TO   (1.)

#define FNAME_LEN (64)

static double my_fun(double x)
{
    return 4. / (x*x + 1.);
}

static void worker_task(int wid, int n_proc, int n_cells)
{
    double result = 0.;
    for (int i = wid; i < n_cells; i += n_proc)
    {
        double x = X_FROM + (X_TO - X_FROM) * ((double) i / n_cells);
        result += my_fun(x) * (X_TO - X_FROM) / n_cells;
    }

    char fname[FNAME_LEN];
    sprintf(fname, "w%d.txt", wid);

    FILE *f = fopen(fname, "w");
    if (!f) exit(-1);

    int err = fwrite(&result, sizeof result, 1, f) == 1 ? 0 : -1;
    fclose(f);

    exit(err);
}

static int compute(int n_proc, int n_cells)
{
    int err = 0;

    for (int wid = 0; wid < n_proc; ++wid)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            err = -1;
            break;
        }
        if (!pid) worker_task(wid, n_proc, n_cells);
    }

    int status;
    while (wait(&status) >= 0)
    {
        if (!WIFEXITED(status) || WEXITSTATUS(status))
        {
            err = -1;
        }
    }

    return err;
}

static int reduce(double *out, int n_proc)
{
    double val;
    char fname[FNAME_LEN];
    *out = 0;

    for (int wid = 0; wid < n_proc; ++wid)
    {
        sprintf(fname, "w%d.txt", wid);

        FILE *f = fopen(fname, "r");
        if (!f) return -1;

        int err = fread(&val, sizeof val, 1, f) == 1 ? 0 : -1;
        fclose(f);
        if (err) return -1;

        *out += val;
    }
    return 0;
}

int run_calculator(double dx, int n_proc)
{
    int err = 0;

    do
    {
        int n_cells = (int) ((X_TO - X_FROM) / dx);
        err = compute(n_proc, n_cells);
        if (err) break;

        double result;
        err = reduce(&result, n_proc);
        if (err) break;

        printf("Result = %lf\n", result);
    } while (0);

    for (int wid = 0; wid < n_proc; ++wid)
    {
        char fname[FNAME_LEN];
        sprintf(fname, "w%d.txt", wid);
        unlink(fname);
    }

    return err;
}
