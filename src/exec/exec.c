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
#include "expand/expand.h"
#include "redir_exec.h"

static int is_builtin(char **words)
{
    if (!words || !words[0])
        return 0;
    return !strcmp(words[0], "true") || !strcmp(words[0], "false")
        || !strcmp(words[0], "echo") || !strcmp(words[0], "exit")
        || !strcmp(words[0], "cd");
}

static int exec_builtin(char **words, int *exit, struct dictionnary *vars)
{
    char *cmd = words[0];
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
    return -1;
}

static char **var_expand(struct dictionnary *vars, char *key, char **res,
    size_t *i)
{
    size_t ind = 0;
    while(res[ind])
    {
        ind++;
    }
    char **val = get_var(vars, key+1);
    size_t j = 0;
    while(val[j]!=NULL)
    {
        res[ind] = strdup(val[j]);
        res[ind][strlen(val[j])] = 0;
        free(val[j]);
        ind++;
        j++;
        res = realloc(res,(ind+1)*sizeof(char*));
        res[ind] = NULL;
    }
    *i+=j;
    free(val);
    return res;
}

static int is_word(char c)
{
    return (c!=' ' && c!='\n' && c!='\t' && c!=';' && c!='&' && c!='|' && c!='\'' && c!=0);
}

/* Description:
 *  	insert all vals value in between prefix and tail
 * Arguments:
 *  	TODO
 */
static void glue(char *prefix, char **vals, char *tail)
{
    size_t i = 0;
    char *res = malloc(1);
    *res = 0;
    while(vals[i])
    {
        size_t len = strlen(res) + strlen(vals[i]) + 1;
        res = realloc(res, len);
        strcat(res, " ");
        strcat(res, vals[i]);
        i++;
    }
    size_t len = strlen(prefix) + strlen(res) + strlen(tail) + 1;
    prefix = realloc(prefix, len);
    strcat(prefix, res);
    strcat(prefix, tail);
    free(res);
    free(tail);
}

static char **double_quotes_expand(struct dictionnary *vars, char *word, char **res)
{
    size_t ind = 0;
    while(res[ind])
    {
        ind++;
    }
    size_t j = 0;
    while(word[j] != 0)
    {
        j++;
    }
    if(j>2)
    {
        res[ind] = malloc((j-1));
        j = 1;
        size_t indbis = 0;
        size_t len = strlen(word);
        while(j < len - 1)
        {
            res[ind][indbis] = word[j];
            indbis++;
            j++;
        }
        res[ind][indbis] = 0;
        ind++;
        res = realloc(res, (ind+1)*sizeof(char*));
        res[ind] = NULL;
    }
    ind--;
    size_t i = 0;
    while(res[ind][i]!=0)
    {
        if(res[ind][i] == '$')
        {
            res[ind][i] = 0;
            i++;
            char *var = (res[ind])+i;
            while(is_word(res[ind][i]))
            {
                i++;
            }
            res[ind][i] = 0;
            char **val = get_var(vars,var);
            char *tail = strdup((res[ind])+i+1);
            glue(res[ind],val, tail);
        }
        else
            i++;
    }
    return res;
}

static void free_ex(char **ex)
{
    size_t i = 0;
    while (ex[i])
    {
        free(ex[i]);
        i++;
    }
    free(ex);
}

static char **expand(struct dictionnary *vars, char **words)
{
    char **res = malloc(sizeof(char *));
    if (!res)
        return NULL;
    res[0] = NULL;
    size_t i = 0;
    size_t j = 0;
    while (words[i]!=NULL)
    {
        if(words[i][0] == '$')
        {
            res = var_expand(vars,(words[i]),res,&j);
        }
        else if(words[i][0] == '\'')
        {
            size_t ind = 0;
            while(words[i][ind] != 0)
            {
                ind++;
            }
            if(ind>2)
            {
            res[j] = malloc((ind-1));
            ind = 0;
            size_t indbis = 0;
            while(words[i][ind] != 0)
            {
                if(words[i][ind] != '\'')
                {
                    res[j][indbis] = words[i][ind];
                    indbis++;
                }
                ind++;
            }
            res[j][indbis] = 0;
            j++;
            res = realloc(res, (j+1)*sizeof(char*));
            res[j] = NULL;
            }
        }
        else if(words[i][0] == '"')
        {
            res = double_quotes_expand(vars,words[i],res);
            j++;
        }
        else
        {
            res[j] = strdup(words[i]);
            j++;
            res = realloc(res, (j+1)*sizeof(char*));
            if(!res)
            {
                free_ex(res);
                return NULL;
            }   
            res[j] = NULL;
        }
        i++;
    }
    return res;
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

    char **expanded = expand(vars, ast_cmd->words);
    if (!expanded)
        return 1;

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

    free_ex(expanded);
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }
    return 127;
}

int exec_pipe(struct ast_cmd **cmd, int fd[2], struct dictionnary *vars, int *exit)
{
    if (cmd[0] == NULL)
        return 2;
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
