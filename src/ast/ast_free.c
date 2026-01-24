#include <stdlib.h>

#include "ast_aux.h"

void ast_free_cmd(struct ast *ast)
{
    int i = 0;
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;
    while (ast_cmd->assignment[i])
    {
        free(ast_cmd->assignment[i]);
        i++;
    }
    free(ast_cmd->assignment);
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
        if (ast_cmd->redirs[i]->io_num)
            free(ast_cmd->redirs[i]->io_num);
        free(ast_cmd->redirs[i]->target);
        free(ast_cmd->redirs[i]);
        i++;
    }
    free(ast_cmd->redirs);
    free(ast_cmd);
}

void ast_free_if(struct ast *ast)
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

void ast_free_list(struct ast *ast)
{
    struct ast_list *ast_list = (struct ast_list *)ast;
    if (ast_list->elt)
        free_ast(ast_list->elt);
    if (ast_list->next)
        ast_free_list((struct ast *)ast_list->next);
    free(ast_list);
}

void ast_free_loop(struct ast *ast)
{
    struct ast_loop *ast_loop = (struct ast_loop *)ast;
    if (ast_loop->condition)
        free_ast(ast_loop->condition);
    if (ast_loop->body)
        free_ast(ast_loop->body);
    free(ast_loop);
}

void ast_free_for(struct ast *ast)
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

void ast_free_pipe(struct ast *ast)
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

void ast_free_and_or(struct ast *ast)
{
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    if (ast_and_or->left)
        free_ast(ast_and_or->left);
    if (ast_and_or->right)
        free_ast(ast_and_or->right);
    free(ast_and_or);
}

void ast_free_shell_redir(struct ast *ast)
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
