#include <stdlib.h>

#include "parser_aux.h"

/*
 * Description:
 * 	Handle a shell command block by calling corresponding parser at each step
 * Return:
 *
 * Verbose:
 * 	Grammar:
 * 		either an if, while or until block
 */
struct ast *parser_shell_command(struct lex *lex)
{
    struct ast *ast = NULL;
    if (peek(lex) && peek(lex)->token_type == IF)
        ast = parser_rule_if(lex);
    else if (peek(lex) && peek(lex)->token_type == WHILE)
        ast = parser_rule_while(lex);
    else
        ast = parser_rule_until(lex);

    if (peek(lex)
        && (peek(lex)->token_type == IO_NUMBER
            || is_redir(peek(lex)->token_type)))
    {
        struct ast_shell_redir *wrapper =
            (struct ast_shell_redir *)init_ast_shell_redir();
        if (!wrapper)
        {
            free_ast(ast);
            return NULL;
        }
        wrapper->child = ast;
        while (peek(lex)
               && (peek(lex)->token_type == IO_NUMBER
                   || is_redir(peek(lex)->token_type)))
        {
            int error = parser_redir(lex, (struct ast_cmd *)wrapper);
            if (error)
            {
                free_ast((struct ast *)wrapper);
                return NULL;
            }
        }
        return (struct ast *)wrapper;
    }
    return ast;
}

/*
 * Description:
 * 	Group words in a command in order to form a list of commands
 * Return:
 * 	*ast -> ast containing a command to execute
 * Verbose:
 * 	Grammar:
 * 		{prefix} WORD {element}
 * 		| prefix {prefix}
 */
// prochaine step -> ajouter gestion des préfixes ( cf. Trove Shell Syntax )
struct ast *parser_simple_command(struct lex *lex)
{
    struct ast_cmd *ast_cmd = (struct ast_cmd *)init_ast_cmd();
    size_t w = 0;
    while (peek(lex)
           && (peek(lex)->token_type == IO_NUMBER
               || is_redir(peek(lex)->token_type)
               || peek(lex)->token_type == ASSIGNMENT))
    {
        if (parser_prefix(lex, ast_cmd))
        {
            goto ERROR;
        }
    }
    lex->context = WORD;
    if (peek(lex)
        && (peek(lex)->token_type == WORD
            || peek(lex)->token_type == EXPANSION))
    {
        struct token *tok = pop(lex);
        if (!tok)
            goto ERROR;
        ast_cmd->words[w] = tok->value;
        ast_cmd->types = realloc(ast_cmd->types, (w + 1) * sizeof(enum type));
        if (!ast_cmd->types)
        {
            goto ERROR;
        }
        ast_cmd->types[w] = tok->token_type;
        free(tok);
        w++;
        ast_cmd->words = realloc(ast_cmd->words, (w + 1) * sizeof(char *));
        if (!ast_cmd->words)
        {
            goto ERROR;
        }
        ast_cmd->words[w] = NULL;

        while (peek(lex) != NULL
               && (peek(lex)->token_type == IO_NUMBER
                   || peek(lex)->token_type == WORD
                   || is_redir(peek(lex)->token_type)
                   || peek(lex)->token_type == EXPANSION))
        {
            if (parser_element(lex, ast_cmd, &w))
            {
                goto ERROR;
            }
        }
    }

    lex->context = KEYWORD;
    if ((ast_cmd->redirs && ast_cmd->redirs[0])
        || (ast_cmd->assignment && ast_cmd->assignment[0]) || w > 0)
    {
        return (struct ast *)ast_cmd;
    }

ERROR:
    free_ast((struct ast *)ast_cmd);
    return NULL;
}

// prochaine step -> ajouter gestion  { redirections } après shell_command
struct ast *parser_command(struct lex *lex)
{
    if (peek(lex)
        && (peek(lex)->token_type == IF || peek(lex)->token_type == WHILE
            || peek(lex)->token_type == UNTIL))
    {
        return parser_shell_command(lex);
    }
    return parser_simple_command(lex);
}
