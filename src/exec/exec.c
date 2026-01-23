#define _POSIX_C_SOURCE 200809L
#include "exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../builtin/echo.h"
#include "../builtin/exit.h"
#include "../builtin/cd.h"
#include "../builtin/export.h"
#include "../builtin/dot.h"
#include "../builtin/unset.h"
#include "../builtin/break_continue.h"
#include "../utils/itoa.h"
#include "redir_exec.h"
#include "../utils/redir.h"

static int is_builtin(char **words)
{
    if (!words || !words[0])
        return 0;
    return !strcmp(words[0], "true") || !strcmp(words[0], "false")
        || !strcmp(words[0], "echo") || !strcmp(words[0], "exit")
        || !strcmp(words[0], "cd") || !strcmp(words[0], "break")
        || !strcmp(words[0], "continue") || !strcmp(words[0], ".")
		|| !strcmp(words[0], "unset") || !strcmp(words[0],"export");
}

static int exec_builtin(char **words, int *exit, struct dictionnary *vars)
{
    char *cmd = words[0];
    int res;
    if (!strcmp(cmd, "true"))
        return 0;
    else if (!strcmp(cmd, "false"))
        return 1;
    else if (!strcmp(cmd, "echo"))
        return echo_b(words + 1);
    else if (!strcmp(cmd, "exit"))
        return exit_b(words + 1, exit);
    else if (!strcmp(cmd, "cd"))
        return cd_b(words + 1, vars);
    else if (!strcmp(cmd, "break"))
    {
        res = break_b(words + 1);
        if (res == 128)
        {
            *exit = 1;
            fprintf(stderr, "Break error\n");
        }
        return res;
    }
    else if (!strcmp(cmd, "continue"))
    {
        res = continue_b(words + 1);
        if (res == 128)
        {
            *exit = 1;
            fprintf(stderr, "Continue error\n");
        }
        return res;
    }
    else if (!strcmp(cmd, "."))
	    return dot_b(words + 1, vars, exit);
    else if(!strcmp(cmd, "unset"))
    {
	    if(!strcmp(words[1], "-v"))
		    return unset(vars, words + 2);
	    else
		    return unset(vars, words + 1);
    }
    else if (!strcmp(cmd, "export"))
        return export_b(words + 1, vars);
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
int exec_cmd(struct ast_cmd *ast_cmd, struct dictionnary *vars, int *exit)
{
    if (*exit)
        return 0;

    if (!ast_cmd->words
        || (!ast_cmd->words[0] && !ast_cmd->redirs && !ast_cmd->assignment[0]))
        return 2;

    if (!ast_cmd->words || !ast_cmd->words[0])
    {
        if (ast_cmd->redirs)
        {
            struct redir_saved redir_saved;
            if (redir_apply(ast_cmd->redirs, &redir_saved))
                return 1;
            restore_redirs(&redir_saved);
        }
        size_t i = 0;
        while (ast_cmd->assignment[i])
        {
            if (add_var(vars, ast_cmd->assignment[i]))
            {
                return 1;
            }
            i++;
        }
        char *wexit = itoa(0);
        char *assignment = malloc(strlen("?=") + strlen(wexit) + 1);
        assignment = strcpy(assignment, "?=");
        assignment = strcat(assignment, wexit);
        add_var(vars, assignment);
        free(wexit);
        free(assignment);
        return 0;
    }

    char **expanded = expand(vars, ast_cmd->words);
    if (!expanded || !expanded[0])
    {
        free_ex(expanded);
        fprintf(stderr, "Command not found\n");
        return 127;
    }

    if (is_builtin(expanded))
    {
        struct redir_saved redir_saved;
        if (redir_apply(ast_cmd->redirs, &redir_saved))
        {
            free_ex(expanded);
            return 1;
        }
        int r = exec_builtin(expanded, exit, vars);
        free_ex(expanded);
        restore_redirs(&redir_saved);
        char *wexit = itoa(r);
        char *assignment = malloc(strlen("?=") + strlen(wexit) + 1);
        assignment = strcpy(assignment, "?=");
        assignment = strcat(assignment, wexit);
        add_var(vars, assignment);
        free(wexit);
        free(assignment);
        return r;
    }

    pid_t pid = fork();

    if (pid == 0)
    {
        struct redir_saved redir_saved;
        if (redir_apply(ast_cmd->redirs, &redir_saved))
            _exit(1);
        execvp(expanded[0], expanded);
        fprintf(stderr, "Command unknown\n");
        free_ex(expanded);
        _exit(127);
    }

    size_t i = 0;
    while (ast_cmd->assignment[i])
    {
        if (add_var(vars, ast_cmd->assignment[i]))
        {
            return 1;
        }
        i++;
    }
    free_ex(expanded);
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
    {
        char *wexit = itoa((int)WEXITSTATUS(status));
        char *assignment = malloc(strlen("?=") + strlen(wexit) + 1);
        assignment = strcpy(assignment, "?=");
        assignment = strcat(assignment, wexit);
        add_var(vars, assignment);
        free(wexit);
        free(assignment);
        return WEXITSTATUS(status);
    }
    char *wexit = itoa(127);
    char *assignment = malloc(strlen("?=") + strlen(wexit) + 1);
    assignment = strcpy(assignment, "?=");
    assignment = strcat(assignment, wexit);
    add_var(vars, assignment);
    free(wexit);
    free(assignment);
    return 127;
}

int exec_pipe(struct ast_cmd **cmd, int fd[2], struct dictionnary *vars, int *exit)
{
    if (cmd[0] == NULL)
        return 0;
    if (cmd[1] == NULL)
    {
        if (fd[0] == 0 && fd[1] == 0)
            return run_ast((struct ast *)cmd[0], vars, exit);
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);
        int res = run_ast((struct ast *)cmd[0], vars, exit);
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
            _exit(run_ast((struct ast *)cmd[0], vars, exit));
        }
        close(fd[1]);
        int w;
        waitpid(child, &w, 0);
        return exec_pipe(cmd + 1, fd, vars, exit);
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
        _exit(run_ast((struct ast *)cmd[0], vars, exit));
    }
    close(fdbis[1]);
    int w;
    waitpid(child, &w, 0);
    return exec_pipe(cmd + 1, fdbis, vars, exit);
}
