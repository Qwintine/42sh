#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../builtin/break_continue.h"
#include "../exec/exec.h"
#include "../exec/redir_exec.h"
#include "ast_aux.h"

int ast_run_cmd(struct ast *ast, struct dictionnary *vars, int *exit)
{
    if (!ast)
        return 2;
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;
    int res = exec_cmd(ast_cmd, vars, exit);
    return res;
}

int ast_run_if(struct ast *ast, struct dictionnary *vars, int *exit)
{
    struct ast_if *ast_if = (struct ast_if *)ast;
    int res = 0;
    if (!run_ast(ast_if->condition, vars, exit))
        res = run_ast(ast_if->then_body, vars, exit);
    else if (ast_if->else_body)
        res = run_ast(ast_if->else_body, vars, exit);
    return res;
}

int ast_run_list(struct ast *ast, struct dictionnary *vars, int *exit)
{
    struct ast_list *ast_list = (struct ast_list *)ast;
    int res = 0;
    if (ast_list->elt)
    {
        res = run_ast(ast_list->elt, vars, exit);
        if (*exit)
            return res;
        if (get_break() > 0 || get_continue() > 0)
            return res;
    }
    if (ast_list->next)
        res = ast_run_list((struct ast *)ast_list->next, vars, exit);
    return res;
}

int ast_run_loop(struct ast *ast, struct dictionnary *vars, int *exit)
{
    struct ast_loop *ast_loop = (struct ast_loop *)ast;
    int res = 0;
    int loop_res = 0;
    while (run_ast(ast_loop->condition, vars, exit) == ast_loop->truth)
    {
        if (*exit)
            return res;

        res = run_ast(ast_loop->body, vars, exit);
        if (*exit)
            return res;

        if (get_break() > 0)
        {
            if (res != 0)
                loop_res = res;
            set_break();
            break;
        }
        if (get_continue() > 0)
        {
            if (res != 0)
                loop_res = res;
            set_continue();
            continue;
        }
    }
    if (loop_res != 0)
        return loop_res;
    return res;
}

static void clean_vars(char **vars, size_t i)
{
    for (size_t j = i; vars[j] != NULL; j++)
        free(vars[j]);
    free(vars);
}

static int break_continue(int *b_c, int res)
{
    if (get_break() > 0)
    {
        if (res != 0)
            *b_c = res;
        set_break();
        return 1;
    }
    if (get_continue() > 0)
    {
        if (res != 0)
            *b_c = res;
        set_continue();
        return 0;
    }
    return -1;
}

int ast_run_for(struct ast *ast, struct dictionnary *vars, int *exit)
{
    struct ast_for *ast_for = (struct ast_for *)ast;
    int res = 0;
    int b_c = 0;
    if (!ast_for->words[0])
    {
        return 0;
    }
    else
    {
        char **all_vars = expand(vars, ast_for->words);
        if (all_vars[0] == NULL)
        {
            free(all_vars);
            return 0;
        }
        for (size_t i = 0; all_vars[i] != NULL; i++)
        {
            // varas = var_assignment, format "<name>=<value>"
            char *varas = malloc(strlen(ast_for->var) + strlen("=")
                                 + strlen(all_vars[i]) + 1);
            if (!varas)
                return 1;
            strcpy(varas, ast_for->var);
            strcat(varas, "=");
            strcat(varas, all_vars[i]);
            add_var(vars, varas);
            res = run_ast(ast_for->body, vars, exit);
            free(varas);
            free(all_vars[i]);

            if (*exit)
            {
                clean_vars(all_vars, i + 1);
                return res;
            }

            int control = break_continue(&b_c, res);
            if (control == 1)
            {
                clean_vars(all_vars, i + 1);
                if (b_c != 0)
                    return b_c;
                return res;
            }
            if (control == 0)
                continue;
        }
        free(all_vars);
    }
    if (b_c != 0)
        return b_c;
    return res;
}

int ast_run_pipe(struct ast *ast, struct dictionnary *vars, int *exit)
{
    struct ast_pipe *ast_pipe = (struct ast_pipe *)ast;
    int fd[2] = { 0, 0 };
    int res = exec_pipe(ast_pipe->cmd, fd, vars, exit);
    if (ast_pipe->negation)
    {
        res = !res;
    }
    return res;
}

int ast_run_and_or(struct ast *ast, struct dictionnary *vars, int *exit)
{
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    int res = run_ast(ast_and_or->left, vars, exit);

    if (*exit)
        return res;

    if (ast_and_or->operator== AND)
    {
        if (res == 0)
            res = run_ast(ast_and_or->right, vars, exit);
    }
    else if (ast_and_or->operator== OR)
    {
        if (res != 0)
            res = run_ast(ast_and_or->right, vars, exit);
    }

    return res;
}

int ast_run_shell_redir(struct ast *ast, struct dictionnary *vars,
                               int *exit)
{
    struct ast_shell_redir *ast_shell = (struct ast_shell_redir *)ast;
    struct redir_saved redir_saved;
    if (redir_apply(ast_shell->redirs, &redir_saved))
        return 1;
    int res = run_ast(ast_shell->child, vars, exit);
    restore_redirs(&redir_saved);
    return res;
}
