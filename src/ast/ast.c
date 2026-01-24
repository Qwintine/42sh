#include "ast_aux.h"

int run_ast(struct ast *ast, struct dictionnary *vars, int *exit)
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
    return ((*functions[ast->type])(ast, vars, exit));
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
