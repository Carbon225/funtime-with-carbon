#include "executor.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static int execute_command(const char *cmd, int in_fd)
{
    int out_fds[2];
    pipe(out_fds);

    pid_t pid = fork();
    if (!pid)
    {
        close(out_fds[0]);
        dup2(in_fd, STDIN_FILENO);
        dup2(out_fds[1], STDOUT_FILENO);
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        exit(0);
    }
    close(out_fds[1]);

    return out_fds[0];
}

static int execute_assign(assign_expr_t *ex, int in_fd)
{
    int last_out_fd = in_fd;
    for (int i = 0; i < ex->num_commands; ++i)
    {
        last_out_fd = execute_command(ex->commands[i], last_out_fd);
    }
    return last_out_fd;
}

int program_execute(program_t *prog)
{
    for (int iex = 0; iex < prog->num_exec_exprs; ++iex)
    {
        exec_expr_t *exec_expr = &prog->exec_exprs[iex];

        int last_assign_out_fd = STDIN_FILENO;

        for (int ias = 0; ias < exec_expr->num_symbols; ++ias)
        {
            assign_expr_t *assign_expr = &prog->assign_exprs[exec_expr->symbols[ias]];

            last_assign_out_fd = execute_assign(assign_expr, last_assign_out_fd);
        }

        for (;;)
        {
            char c;
            int err = read(last_assign_out_fd, &c, 1) == 1 ? 0 : 1;
            if (err) break;
            write(STDOUT_FILENO, &c, 1);
        }

        while (!wait(NULL));
    }
    return 0;
}
