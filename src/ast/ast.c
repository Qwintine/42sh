#include "ast.h"

#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../exec/exec.h"
#include "../exec/redir_exec.h"

//===================== Init ast from specific type ===========================

struct ast *init_ast_list(void)
{
    struct ast_list *node = malloc(sizeof(struct ast_list));
    if (!node)
        return NULL;
    node->base.type = AST_LIST;
    node->elt = NULL;
    node->next = NULL;
    return (struct ast *)node;
}

struct ast *init_ast_if(void)
{
    struct ast_if *node = malloc(sizeof(struct ast_if));
    if (!node)
        return NULL;
    node->base.type = AST_IF;
    node->condition = NULL;
    node->then_body = NULL;
    node->else_body = NULL;
    return (struct ast *)node;
}

struct ast *init_ast_cmd(void)
{
    struct ast_cmd *node = malloc(sizeof(struct ast_cmd));
    if (!node)
        return NULL;
    node->base.type = AST_CMD;
    node->assignment = calloc(1, sizeof(char *));
    if (!node->assignment)
    {
        free(node);
        return NULL;
    }
    node->types = NULL;
    node->words = calloc(1, sizeof(char *));
    if (!node->words)
    {
        free(node);
        return NULL;
    }
    node->redirs = calloc(1, sizeof(struct redir *));
    if (!node->redirs)
    {
        free(node->words);
        free(node);
        return NULL;
    }
    return (struct ast *)node;
}

struct ast *init_ast_loop(void)
{
    struct ast_loop *node = malloc(sizeof(struct ast_loop));
    if (!node)
        return NULL;
    node->base.type = AST_LOOP;
    node->truth = 0;
    node->condition = NULL;
    node->body = NULL;
    return (struct ast *)node;
}

struct ast *init_ast_pipe(void)
{
    struct ast_pipe *node = malloc(sizeof(struct ast_pipe));
    if (!node)
        return NULL;
    node->base.type = AST_PIPE;
    node->negation = 0;
    node->cmd = malloc(sizeof(struct ast_cmd *));
    node->cmd[0] = NULL;
    return (struct ast *)node;
}

struct ast *init_ast_and_or(void)
{
    struct ast_and_or *node = malloc(sizeof(struct ast_and_or));
    if (!node)
        return NULL;
    node->base.type = AST_AND_OR;
    node->left = NULL;
    node->right = NULL;
    node->operator= END;
    return (struct ast *)node;
}

struct ast *init_ast_shell_redir(void)
{
    struct ast_shell_redir *node = malloc(sizeof(struct ast_shell_redir));
    if (!node)
        return NULL;
    node->base.type = AST_SHELL_REDIR;
    node->words = NULL;
    node->child = NULL;
    node->redirs = calloc(1, sizeof(struct redir *));
    if (!node->redirs)
    {
        free(node);
        return NULL;
    }
    return (struct ast *)node;
}

struct ast *init_ast_for(void)
{
    struct ast_for *node = malloc(sizeof(struct ast_for));
    if (!node)
        return NULL;
    node->base.type = AST_FOR;
    node->var = NULL;
    node->words = malloc(sizeof(char *));
    node->words[0] = NULL;
    node->body = NULL;
    return (struct ast *)node;
}

//===================== Free ast from specific type ===========================

static void ast_free_cmd(struct ast *ast)
{
    int i = 0;
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;
    while (ast_cmd->assignment[i])
    {
        free(ast_cmd->assignment[i]);
        i++;
    }
    free(ast_cmd->assignment);
    free(ast_cmd->types);
    i = 0;
    while (ast_cmd->words[i])
    {
        free(ast_cmd->words[i]);
        i++;
    }
    free(ast_cmd->words);
    i = 0;
    while (ast_cmd->redirs[i])
    {
        free(ast_cmd->redirs[i]->target);
        free(ast_cmd->redirs[i]);
        i++;
    }
    free(ast_cmd->redirs);
    free(ast_cmd);
}

static void ast_free_if(struct ast *ast)
{
    struct ast_if *ast_if = (struct ast_if *)ast;
    if (ast_if->condition)
        free_ast(ast_if->condition);
    if (ast_if->then_body)
        free_ast(ast_if->then_body);
    if (ast_if->else_body)
        free_ast(ast_if->else_body);
    free(ast_if);
}

static void ast_free_list(struct ast *ast)
{
    struct ast_list *ast_list = (struct ast_list *)ast;
    if (ast_list->elt)
        free_ast(ast_list->elt);
    if (ast_list->next)
        ast_free_list((struct ast *)ast_list->next);
    free(ast_list);
}

static void ast_free_loop(struct ast *ast)
{
    struct ast_loop *ast_loop = (struct ast_loop *)ast;
    if (ast_loop->condition)
        free_ast(ast_loop->condition);
    if (ast_loop->body)
        free_ast(ast_loop->body);
    free(ast_loop);
}

static void ast_free_for(struct ast *ast)
{
    struct ast_for *ast_for = (struct ast_for *)ast;
    if (ast_for->var)
        free(ast_for->var);
    if (ast_for->words)
    {
        for (size_t i = 0; ast_for->words[i] != NULL; i++)
        {
            free(ast_for->words[i]);
        }
        free(ast_for->words);
    }
    if (ast_for->body)
        free_ast(ast_for->body);
    free(ast_for);
}

static void ast_free_pipe(struct ast *ast)
{
    struct ast_pipe *ast_pipe = (struct ast_pipe *)ast;
    if (ast_pipe->cmd)
    {
        for (size_t i = 0; ast_pipe->cmd[i] != NULL; i++)
        {
            free_ast((struct ast *)ast_pipe->cmd[i]);
        }
        free(ast_pipe->cmd);
    }
    free(ast_pipe);
}

static void ast_free_and_or(struct ast *ast)
{
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    if (ast_and_or->left)
        free_ast(ast_and_or->left);
    if (ast_and_or->right)
        free_ast(ast_and_or->right);
    free(ast_and_or);
}

static void ast_free_shell_redir(struct ast *ast)
{
    struct ast_shell_redir *ast_shell = (struct ast_shell_redir *)ast;
    if (ast_shell->child)
        free_ast(ast_shell->child);
    int i = 0;
    while (ast_shell->redirs[i])
    {
        free(ast_shell->redirs[i]->target);
        free(ast_shell->redirs[i]);
        i++;
    }
    free(ast_shell->redirs);
    free(ast_shell);
}

//===================== Run ast from specific type =============================

// TODO adapter à redir
static int ast_run_cmd(struct ast *ast, struct dictionnary *vars)
{
    if (!ast)
        return 2;
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;
    int res = exec_cmd(ast_cmd, vars);
    return res;
}

static int ast_run_if(struct ast *ast, struct dictionnary *vars)
{
    struct ast_if *ast_if = (struct ast_if *)ast;
    int res = 0;
    if (!run_ast(ast_if->condition, vars))
        res = run_ast(ast_if->then_body, vars);
    else if (ast_if->else_body)
        res = run_ast(ast_if->else_body, vars);
    return res;
}

static int ast_run_list(struct ast *ast, struct dictionnary *vars)
{
    struct ast_list *ast_list = (struct ast_list *)ast;
    int res = 0;
    if (ast_list->elt)
        res = run_ast(ast_list->elt, vars);
    if (ast_list->next)
        res = ast_run_list((struct ast *)ast_list->next, vars);
    return res;
}

static int ast_run_loop(struct ast *ast, struct dictionnary *vars)
{
    struct ast_loop *ast_loop = (struct ast_loop *)ast;
    int res = 0;
    if (run_ast(ast_loop->condition, vars) == ast_loop->truth)
        res = run_ast(ast_loop->body, vars);
    return res;
}

static int ast_run_for(struct ast *ast, struct dictionnary *vars)
{
    struct ast_for *ast_for = (struct ast_for *)ast;
    int res = 0;
    if (!ast_for->words[0])
    {
        //varas = var_assignment, format "<name>=<value>"
        //in this case, the value is empty
        /*char *varas = calloc(strlen(ast_for->var) + strlen("=") + 1,1);
        if (!varas)
            return 1;
        strcpy(varas, ast_for->var);
        strcat(varas, "=");
        add_var(vars, varas);
        res = run_ast(ast_for->body, vars);
        free(varas);*/
        return 0;
    }
    else
    {
        for (size_t i = 0; ast_for->words[i] != NULL; i++)
        {
            //varas = var_assignment, format "<name>=<value>"
            char *varas = malloc(strlen(ast_for->var) + strlen("=") + strlen(ast_for->words[i]) + 1);
            if (!varas)
                return 1;
            strcpy(varas, ast_for->var);
            strcat(varas, "=");
            strcat(varas,  ast_for->words[i]);
            add_var(vars, varas);
            res = run_ast(ast_for->body, vars);
            free(varas);
        }
    }
    return res;
}

static int ast_run_pipe(struct ast *ast, struct dictionnary *vars)
{
    if (!ast)
        return 2;
    struct ast_pipe *ast_pipe = (struct ast_pipe *)ast;
    if (!ast_pipe->cmd[0])
        return 2;
    int fd[2] = { 0, 0 };
    int res = exec_pipe(ast_pipe->cmd, fd, vars);
    if (ast_pipe->negation)
    {
        res = !res;
    }
    return res;
}

static int ast_run_and_or(struct ast *ast, struct dictionnary *vars)
{
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    int res = run_ast(ast_and_or->left, vars);

    if (ast_and_or->operator== AND)
    {
        if (res == 0)
            res = run_ast(ast_and_or->right, vars);
    }
    else if (ast_and_or->operator== OR)
    {
        if (res != 0)
            res = run_ast(ast_and_or->right, vars);
    }

    return res;
}

static int ast_run_shell_redir(struct ast *ast, struct dictionnary *vars)
{
    struct ast_shell_redir *ast_shell = (struct ast_shell_redir *)ast;
    struct redir_saved redir_saved;
    if (redir_apply(ast_shell->redirs, &redir_saved))
        return 1;
    int res = run_ast(ast_shell->child, vars);
    restore_redirs(&redir_saved);
    return res;
}

//=========================== Lookup Tables ===================================

int run_ast(struct ast *ast, struct dictionnary *vars)
{
    static const ast_handler_run functions[] = {
        [AST_LOOP] = &ast_run_loop,
        [AST_CMD] = &ast_run_cmd,
        [AST_IF] = &ast_run_if,
        [AST_LIST] = &ast_run_list,
        [AST_PIPE] = &ast_run_pipe,
        [AST_AND_OR] = &ast_run_and_or,
        [AST_SHELL_REDIR] = &ast_run_shell_redir,
        [AST_FOR] = &ast_run_for,
    };
    return ((*functions[ast->type])(ast, vars));
}

void free_ast(struct ast *ast)
{
    static const ast_handler_free functions[] = {
        [AST_LOOP] = &ast_free_loop,
        [AST_CMD] = &ast_free_cmd,
        [AST_IF] = &ast_free_if,
        [AST_LIST] = &ast_free_list,
        [AST_PIPE] = &ast_free_pipe,
        [AST_AND_OR] = &ast_free_and_or,
        [AST_SHELL_REDIR] = &ast_free_shell_redir,
        [AST_FOR] = &ast_free_for,
    };
    (*functions[ast->type])(ast);
}
