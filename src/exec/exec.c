#define _POSIX_C_SOURCE 200809L
#include "exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../builtin/break_continue.h"
#include "../builtin/cd.h"
#include "../builtin/dot.h"
#include "../builtin/echo.h"
#include "../builtin/exit.h"
#include "../builtin/export.h"
#include "../builtin/unset.h"
#include "../utils/itoa.h"
#include "../utils/redir.h"
#include "redir_exec.h"

/* Description:
 * 	Check if cmd is builtin
 * Arguments:
 * 	char **words -> args passed to builtin
 * Return:
 * 	int -> 1 if builtin, 0 otherwise
 */
static int is_builtin(char **words)
{
    if (!words || !words[0])
        return 0;
    return !strcmp(words[0], "true") || !strcmp(words[0], "false")
        || !strcmp(words[0], "echo") || !strcmp(words[0], "exit")
        || !strcmp(words[0], "cd") || !strcmp(words[0], "break")
        || !strcmp(words[0], "continue") || !strcmp(words[0], ".")
        || !strcmp(words[0], "unset") || !strcmp(words[0], "export");
}

/* Description:
 * 	Execute builtin cmd with given args
 * Arguments:
 * 	char **words -> args passed to builtin
 * 	int *exit -> pointer to exit status
 * 	struct dictionnary *vars -> dictionnary of vars
 * Return:
 * 	int -> Exit status of builtin cmd
 */
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
    else if (!strcmp(cmd, "unset"))
    {
        if (words[1] && !strcmp(words[1], "-v"))
            return unset(vars, words + 2);
        else
            return unset(vars, words + 1);
    }
    else if (!strcmp(cmd, "export"))
        return export_b(words + 1, vars);
    return -1;
}

/* Description:
 * 	Execute funct with given arguments
 * Arguments:
 * 	struct ast *func -> AST node of funct
 * 	struct ast_cmd *ast_cmd -> AST node of cmd
 * 	struct dictionnary *vars -> dictionnary of vars
 * Return:
 * 	int -> Exit status of funct
 */
static int exec_func(struct ast *func, struct ast_cmd *ast_cmd,
                     struct dictionnary *vars)
{
    char **expanded = expand(vars, ast_cmd->words);
    char **old_params[10] = { NULL };
    char key[2] = "0";
    int max_param = 0;

    for (int i = 1; expanded && expanded[i] && i < 10; i++)
    {
        key[0] = '0' + i;
        old_params[i] = get_var(vars, key);
        char **val = malloc(2 * sizeof(char *));
        val[0] = expanded[i];
        val[1] = NULL;
        add_var_arg(vars, key, val);
        free(val);
        max_param = i;
    }

    int func_exit = 0;
    int res = run_ast(func, vars, &func_exit);

    for (int i = 1; i <= max_param; i++)
    {
        key[0] = '0' + i;
        char **current = get_var(vars, key);
        if (current)
        {
            free(current[0]);
            free(current);
        }

        if (old_params[i] && old_params[i][0])
            add_var_arg(vars, key, old_params[i]);
        else if (old_params[i])
            free(old_params[i]);
    }
    if (expanded)
    {
        for (int i = 0; expanded[i]; i++)
            free(expanded[i]);
        free(expanded);
    }
    return res;
}
/* Description:
 * 	Update special var "?" with given status
 * Arguments:
 * 	struct dictionnary *vars -> dictionnary of variables
 * 	int status -> status to set "?" to
 */
static void update_exit(struct dictionnary *vars, int status)
{
    char *wexit = itoa(status);
    if (wexit)
    {
        char *assignment = malloc(strlen("?=") + strlen(wexit) + 1);
        if (assignment)
        {
            assignment = strcpy(assignment, "?=");
            assignment = strcat(assignment, wexit);
            add_var(vars, assignment);
            free(assignment);
        }
        free(wexit);
    }
}

/* Description:
 * 	Exec assignments in cmd
 * Arguments:
 * 	struct ast_cmd *ast_cmd -> AST node
 * 	struct dictionnary *vars -> dictionnary of vars
 * Return:
 * 	int -> Exit status of assignment exec
 */
static int exec_assignment(struct ast_cmd *ast_cmd, struct dictionnary *vars)
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
            return 1;
        i++;
    }
    update_exit(vars, 0);
    return 0;
}

/* Description:
 * 	Execute the builtin command with given args
 * Arguments:
 * 	struct ast_cmd *ast_cmd -> AST node
 * 	char **expanded -> expanded arguments
 * 	struct dictionnary *vars -> dictionnary of vars
 * 	int *exit -> pointer to exit status
 * Return:
 * 	int -> Exit status of builtin command
 */
static int exec_b(struct ast_cmd *ast_cmd, char **expanded,
                  struct dictionnary *vars, int *exit)
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
    update_exit(vars, r);
    return r;
}
/* Description:
 *  Execute the command with given arguments
 * Arguments:
 * 	struct ast_cmd *ast_cmd-> AST node
 * 	char **expanded -> expanded arguments
 * 	struct dictionnary *vars -> dictionnary of variables
 * Return:
 * 	int ->Exit status of the command
 */
static int exec(struct ast_cmd *ast_cmd, char **expanded,
                struct dictionnary *vars)
{
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
            return 1;
        i++;
    }
    free_ex(expanded);
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
    {
        update_exit(vars, WEXITSTATUS(status));
        return WEXITSTATUS(status);
    }
    update_exit(vars, 127);
    return 127;
}

/* Description:
 *  	execute les commandes avec les args donnes
 * Arguments:
 *  	words: la ligne de commande a executer
 * Retour:
 * 	0 si succes, 1 sinon
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
        return exec_assignment(ast_cmd, vars);

    if (ast_cmd->words[0])
    {
        struct ast *func = get_func(vars, ast_cmd->words[0]);
        if (func)
            return exec_func(func, ast_cmd, vars);
    }

    char **expanded = expand(vars, ast_cmd->words);
    if (!expanded || !expanded[0])
    {
        free_ex(expanded);
        fprintf(stderr, "Command not found\n");
        return 127;
    }
    if (!expanded[1] && ast_cmd->words[1])
    {
        expanded[1] = calloc(1, 1);
        if (!expanded[1])
        {
            free_ex(expanded);
            return 127;
        }
        char **new_expanded = realloc(expanded, 3 * sizeof(char *));
        if (!new_expanded)
        {
            free_ex(expanded);
            return 127;
        }
        expanded = new_expanded;
        expanded[2] = NULL;
    }

    if (is_builtin(expanded))
        return exec_b(ast_cmd, expanded, vars, exit);

    return exec(ast_cmd, expanded, vars);
}

/* Description:
 * 	Execute a pipeline of commands
 * Arguments:
 * 	struct ast_cmd **cmd -> commands to execute
 * 	int fd[2] -> file descriptors for the pipe
 * 	struct dictionnary *vars -> the dictionnary of variables
 * 	int *exit -> pointer to the exit status
 * Return:
 * 	int -> Exit status of last command executed
 */
int exec_pipe(struct ast_cmd **cmd, int fd[2], struct dictionnary *vars,
              int *exit)
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
