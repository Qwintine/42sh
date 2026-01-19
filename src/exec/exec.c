#define _POSIX_C_SOURCE 200809L
#include "exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../builtin/echo.h"
#include "expand/expand.h"
#include "redir_exec.h"

static int is_builtin(char **words)
{
    if (!words || !words[0])
        return 0;
    return !strcmp(words[0], "true") || !strcmp(words[0], "false")
        || !strcmp(words[0], "echo");
}

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

static char **expand(struct dictionnary *vars, enum type *types, char **words)
{
    // saves variable name
    char **res = malloc(sizeof(char *));
    if (!res)
        return NULL;
    res[0] = NULL;
    size_t i = 0;
    size_t r = 0;
    while (words[i])
    {
        if (types[i] == EXPANSION)
        {
            res[r] = words[i];
            r++;
            res = realloc(res, (r + 1) * sizeof(char *));
            if (!res)
                return NULL;
            res[r] = NULL;
            char **val = get_var(vars, words[i]);
            if (!val)
            {
                words[i] = strdup("");
            }
            else
            {
                words[i] = strdup(val[0]);
                free(val);
            }
        }
        i++;
    }
    return res;
}

// reverse the expansion of variables in words
// so that they can be expanded again later if needed
static void unexpand(enum type *types, char **words, char **res)
{
    size_t i = 0;
    size_t r = 0;
    while (words[i])
    {
        if (types[i] == EXPANSION)
        {
            free(words[i]);
            words[i] = res[r];
            r++;
        }
        i++;
    }
    free(res);
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
int exec_cmd(struct ast_cmd *ast_cmd, struct dictionnary *vars)
{
    if (!ast_cmd->words
        || (!ast_cmd->words[0] && !ast_cmd->redirs && !ast_cmd->assignment[0]))
        return 2;

    size_t i = 0;

    while (ast_cmd->assignment[i])
    {
        if (add_var(vars, ast_cmd->assignment[i]))
        {
            return 1;
        }
        i++;
    }

    if (!ast_cmd->words || !ast_cmd->words[0])
    {
        if (ast_cmd->redirs)
        {
            struct redir_saved redir_saved;
            if (redir_apply(ast_cmd->redirs, &redir_saved))
                return 1;
            restore_redirs(&redir_saved);
        }
        return 0;
    }

    char **expanded = expand(vars, ast_cmd->types, ast_cmd->words);

    if (is_builtin(ast_cmd->words))
    {
        struct redir_saved redir_saved;
        if (redir_apply(ast_cmd->redirs, &redir_saved))
        {
            unexpand(ast_cmd->types, ast_cmd->words, expanded);
            return 1;
        }
        int r = exec_builtin(ast_cmd->words);
        restore_redirs(&redir_saved);
        unexpand(ast_cmd->types, ast_cmd->words, expanded);
        return r;
    }

    pid_t pid = fork();

    if (pid == 0)
    {
        struct redir_saved redir_saved;
        if (redir_apply(ast_cmd->redirs, &redir_saved))
            _exit(1);
        execvp(ast_cmd->words[0], ast_cmd->words);
        fprintf(stderr, "Command unknown\n");
        _exit(127);
    }

    int status;
    waitpid(pid, &status, 0);
    unexpand(ast_cmd->types, ast_cmd->words, expanded);
    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }
    return 127;
}

int exec_pipe(struct ast_cmd **cmd, int fd[2], struct dictionnary *vars)
{
    if (cmd[0] == NULL)
        return 2;
    if (cmd[1] == NULL)
    {
        if (fd[0] == 0 && fd[1] == 0)
            return run_ast((struct ast *)cmd[0], vars);
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);
        int res = run_ast((struct ast *)cmd[0], vars);
        close(fd[0]);
        return res;
    }
    if (fd[0] == 0 && fd[1] == 0)
    {
        if (pipe(fd) == -1)
            return 1;
        int child = fork();
        if (!child)
        {
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            _exit(run_ast((struct ast *)cmd[0], vars));
        }
        close(fd[1]);
        int w;
        waitpid(child, &w, 0);
        return exec_pipe(cmd + 1, fd, vars);
    }
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    close(fd[1]);
    int fdbis[2];
    if (pipe(fdbis) == -1)
        return 1;
    int child = fork();
    if (!child)
    {
        close(fdbis[0]);
        dup2(fdbis[1], STDOUT_FILENO);
        close(fdbis[1]);
        _exit(run_ast((struct ast *)cmd[0], vars));
    }
    close(fdbis[1]);
    int w;
    waitpid(child, &w, 0);
    return exec_pipe(cmd + 1, fdbis, vars);
}
