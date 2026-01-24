#include <stdlib.h>

#include "ast_aux.h"

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
    if (!node->cmd)
    {
        free(node);
        return NULL;
    }
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
    if (!node->words)
    {
        free(node);
        return NULL;
    }
    node->words[0] = NULL;
    node->body = NULL;
    return (struct ast *)node;
}
