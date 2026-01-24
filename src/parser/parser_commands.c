#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <string.h>

#include "parser_aux.h"
#include "../expand/hashmap.h"

/*
 * Description:
 * 	Handle a shell command block by calling corresponding parser at each step
 * Return:
 *
 * Verbose:
 * 	Grammar:
 * 		either an if, while or until block
 */
struct ast *parser_shell_command(struct lex *lex, struct dictionnary *dict)
{
    struct ast *ast = NULL;
    if (peek(lex) && peek(lex)->token_type == IF)
        ast = parser_rule_if(lex, dict);
    else if (peek(lex) && peek(lex)->token_type == WHILE)
        ast = parser_rule_while(lex, dict);
    else if (peek(lex) && peek(lex)->token_type == UNTIL)
        ast = parser_rule_until(lex, dict);
    else if (peek(lex) && peek(lex)->token_type == FOR)
        ast = parser_rule_for(lex, dict);
    else
        ast = parser_rule_command_block(lex, dict);

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
    if (peek(lex) && peek(lex)->token_type == WORD)
    {
        char *val = NULL;
        struct token *tok = pop(lex);
        if (!tok)
            goto ERROR;
        val = tok->value;
        free(tok);
        ast_cmd->words[w] = val;
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
                   || is_redir(peek(lex)->token_type)))
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

struct ast *parser_fundef(struct lex *lex, struct dictionnary *dict)
{
    printf("Parsing function definition\n");
    if(!peek(lex) || !(peek(lex)->token_type == FUNCTION))
        return NULL;
    char *cmd = strdup(peek(lex)->value);
    discard_token(pop(lex));
    cmd[strlen(cmd)-2] = 0; // remove () at the end

    while(peek(lex) && peek(lex)->token_type == NEWLINE)
        discard_token(pop(lex));

    add_func(dict,cmd,parser_shell_command(lex,dict));

    return get_func(dict,cmd);
}

// prochaine step -> ajouter gestion  { redirections } après shell_command
struct ast *parser_command(struct lex *lex, struct dictionnary *dict)
{
    if (peek(lex)
        && (peek(lex)->token_type == IF || peek(lex)->token_type == WHILE
            || peek(lex)->token_type == UNTIL || peek(lex)->token_type == FOR
            || peek(lex)->token_type == OPENING_BRACKET))
    {
        return parser_shell_command(lex, dict);
    }
    if(peek(lex) && peek(lex)->token_type == FUNCTION)
    {
        return parser_fundef(lex,dict);
    }
    return parser_simple_command(lex);
}
