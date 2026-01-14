#include "exec.h"

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../builtin/echo.h"

static int exec_builtin(char **words)
{
    char *cmd = words[0];
    if (!strcmp(cmd, "true"))
        return 0;
    else if (!strcmp(cmd, "false"))
        return 1;
    else if (!strcmp(cmd, "echo"))
        return echo_b(words + 1);
    return -1;
}

/* Description:
 *  	execute les commandes avec les args donnes
 * Arguments:
 *  	words: la ligne de commande a executer
 * Retour:
 *  	0 si succes, 1 sinon
 * Verbose:
 *  	execute la commande en words[0] via un appelle si builtin, via fork->
 *  	execvp sinon.
 */
int exec_cmd(char **words)
{
    if (!words || !words[0])
        return 2;
    int r = exec_builtin(words);
    if (r == -1)
    {
        int child = fork();
        if (!child)
        {
            execvp(words[0], words);
            fprintf(stderr, "42sh: Error exec\n");
            _exit(127);
        }
        int wstat;
        waitpid(child, &wstat, 0);
        r = WEXITSTATUS(wstat);
    }
    return r;
}

int exec_pipe(struct ast_cmd **cmd, int fd[2])
{
    if (cmd[0] == NULL)
        return 2;
    if (cmd[1] == NULL)
    {
        if (fd[0] == 0 && fd[1] == 0)
            return run_ast((struct ast *)cmd[0]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);
        int res = run_ast((struct ast *)cmd[0]);
        close(fd[0]);
        return res;
    }
    if (fd[0] == 0 && fd[1] == 0)
    {
        pipe(fd);
        int child = fork();
        if (!child)
        {
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            return run_ast((struct ast *)cmd[0]);
        }
        int w;
        waitpid(child, &w, 0);
        return exec_pipe(cmd + 1, fd);
    }
    dup2(fd[0], STDIN_FILENO);
    close(fd[1]);
    close(fd[0]);
    int fdbis[2];
    pipe(fdbis);
    int child = fork();
    if (!child)
    {
        dup2(fdbis[1], STDOUT_FILENO);
        close(fdbis[0]);
        close(fdbis[1]);
        return run_ast((struct ast *)cmd[0]);
    }
    int w;
    waitpid(child, &w, 0);
    return exec_pipe(cmd + 1, fdbis);
}
