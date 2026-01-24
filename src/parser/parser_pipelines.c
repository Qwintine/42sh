#include <stdlib.h>

#include "parser_aux.h"

// Parse a pipeline of commands separated by pipes
// Return NULL on error
// Return ast_pipe on success
struct ast *parser_pipeline(struct lex *lex, struct dictionnary *dict)
{
    struct ast_pipe *ast_pipe = (struct ast_pipe *)init_ast_pipe();
    // while negation tokens
    if (peek(lex) && peek(lex)->token_type == NEGATION)
    {
        ast_pipe->negation = 1;
        discard_token(pop(lex));
    }
    if (peek(lex) && peek(lex)->token_type == PIPE)
    {
        free_ast((struct ast *)ast_pipe);
        return NULL;
    }
    if(peek(lex) && peek(lex)->token_type == END && ast_pipe->negation)
        return (struct ast *)ast_pipe;
    size_t ind = 0;
    int pipe = 1;
    struct ast_cmd *ast_cmd = (struct ast_cmd *)parser_command(lex, dict);
    if (!ast_cmd)
    {
        free_ast((struct ast *)ast_pipe);
        return NULL;
    }
    // while there is a pipe at the end and a command following
    while (ast_cmd && pipe)
    {
        ast_pipe->cmd[ind] = ast_cmd;
        ind++;
        ast_pipe->cmd =
            realloc(ast_pipe->cmd, (ind + 1) * sizeof(struct ast_cmd *));
        ast_pipe->cmd[ind] = NULL;
        pipe = (peek(lex) && peek(lex)->token_type == PIPE);
        if (pipe)
            discard_token(pop(lex));
        while (pipe && peek(lex) && peek(lex)->token_type == NEWLINE)
            discard_token(pop(lex));
        ast_cmd = (struct ast_cmd *)parser_command(lex, dict);
    }
    // There was a pipe but no command after it
    if (pipe) // Error parser_command
    {
        free_ast((struct ast *)ast_pipe);
        return NULL;
    }
    return (struct ast *)ast_pipe;
}

/*
 * Description:
 * 	Parse and_or blocks separated by '&&' or '||'
 * Return:
 * 	*ast -> ast containing and_or blocks
 * Verbose:
 * 	Grammar:
 * 		and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline } ;
 */
struct ast *parser_and_or(struct lex *lex, struct dictionnary *dict)
{
    struct ast *left = parser_pipeline(lex, dict);
    if (!left)
        return NULL;
    // while and_or tokens
    while (peek(lex)
           && (peek(lex)->token_type == AND || peek(lex)->token_type == OR))
    {
        struct ast_and_or *ast_and_or = (struct ast_and_or *)init_ast_and_or();
        if (!ast_and_or)
        {
            free_ast(left);
            return NULL;
        }

        ast_and_or->left = left;
        ast_and_or->operator= peek(lex)->token_type;
        discard_token(pop(lex));

        while (peek(lex) && peek(lex)->token_type == NEWLINE)
        {
            discard_token(pop(lex));
        }
        // appel la deuxième partie de l'and_or
        ast_and_or->right = parser_pipeline(lex, dict);
        if (!ast_and_or->right)
        {
            free_ast((struct ast *)ast_and_or);
            return NULL;
        }

        left = (struct ast *)ast_and_or;
    }

    return left;
}
