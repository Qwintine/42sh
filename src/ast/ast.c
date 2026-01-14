#include "ast.h"

#include <stddef.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "../exec/exec.h"

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
    node->words = calloc(1, sizeof(char *));
    if(!node->words)
    {
	    free(node);
	    return NULL;
    }
    node->redirs = calloc(1, sizeof(struct redir *));
    if(!node->redirs)
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

//===================== Free ast from specific type ===========================

static void ast_free_cmd(struct ast *ast)
{
    int i = 0;
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;
    while (ast_cmd->words[i])
    {
        free(ast_cmd->words[i]);
        i++;
    }
    free(ast_cmd->words);
    i = 0;
    while(ast_cmd->redirs[i])
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

//===================== Run ast from specific type =============================

//TODO adapter à redir
static int ast_run_cmd(struct ast *ast)
{
    if (!ast)
        return 2;
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;
    int res = exec_cmd(ast_cmd->words);
    return res;
}

static int ast_run_if(struct ast *ast)
{
    struct ast_if *ast_if = (struct ast_if *)ast;
    int res = 0;
    if (!run_ast(ast_if->condition))
        res = run_ast(ast_if->then_body);
    else if (ast_if->else_body)
        res = run_ast(ast_if->else_body);
    return res;
}

static int ast_run_list(struct ast *ast)
{
    struct ast_list *ast_list = (struct ast_list *)ast;
    int res = 0;
    if (ast_list->elt)
        res = run_ast(ast_list->elt);
    if (ast_list->next)
        res = ast_run_list((struct ast *)ast_list->next);
    return res;
}

static int ast_run_loop(struct ast *ast)
{
    struct ast_loop *ast_loop = (struct ast_loop *)ast;
    int res = 0;
    if (run_ast(ast_loop->condition) == ast_loop->truth)
        res = run_ast(ast_loop->body);
    return res;
}

static int ast_run_pipe(struct ast *ast)
{
    if (!ast)
        return 2;
    struct ast_pipe *ast_pipe = (struct ast_pipe *)ast;
    if (!ast_pipe->cmd[0])
        return 2;
    int fd[2] = { 0, 0 };
    int res = exec_pipe(ast_pipe->cmd, fd);
    if (ast_pipe->negation)
    {
        res = !res;
    }
    return res;
}

//=========================== Lookup Tables ===================================

int run_ast(struct ast *ast)
{
    static const ast_handler_run functions[] = {
        [AST_LOOP] = &ast_run_loop,
        [AST_CMD] = &ast_run_cmd,
        [AST_IF] = &ast_run_if,
        [AST_LIST] = &ast_run_list,
        [AST_PIPE] = &ast_run_pipe,
    };
    return ((*functions[ast->type])(ast));
}

void free_ast(struct ast *ast)
{
    static const ast_handler_free functions[] = {
        [AST_LOOP] = &ast_free_loop,
        [AST_CMD] = &ast_free_cmd,
        [AST_IF] = &ast_free_if,
        [AST_LIST] = &ast_free_list,
        [AST_PIPE] = &ast_free_pipe,
    };
    (*functions[ast->type])(ast);
}
