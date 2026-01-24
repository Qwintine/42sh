#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast_aux.h"
#include "parser_aux.h"

/*
 * Description:
 * 	Parse the input of our shell into list
 * Arguments:
 * 	FILE *entry -> used to create struct lexer
 * Return:
 * 	struct ast * representing an Abstract Syntax Tree of our input / NULL on
 * Error
 *
 * Verbose:
 * 	Grammar:
 * 		list '\n'
 * 	      | list EOF
 * 	      | '\n'
 * 	      | EOF
 * 	      ;
 * 	TODO
 */
struct ast *parser(FILE *entry, int *eof, struct dictionnary *dict)
{
    struct lex *lex = init_lex(entry);
    lex->context = KEYWORD;
    if (!peek(lex))
    {
        free_lex(lex);
        return NULL;
    }

    while (peek(lex)
           && peek(lex)->token_type
               == NEWLINE) // consomme newline until début code évaluable
    {
        discard_token(pop(lex));
    }

    if (!peek(lex)
        || peek(lex)->token_type == END) // fichier sans code évaluable
    {
        *eof = 1;
        free_lex(lex);
        return init_ast_list();
    }
    if (peek(lex) && peek(lex)->token_type == SEMI_COLON)
    {
        free_lex(lex);
        return NULL;
    }
    struct ast *ast = parser_list(
        lex, dict); // récursion sur ast type list ( cf. parser_list )

    if (!ast) // remontée erreur syntax / grammaire
    {
        free_lex(lex);
        return NULL;
    }

    if (!peek(lex)
        || (peek(lex)->token_type != NEWLINE && peek(lex)->token_type != END))
    {
        free_lex(lex);
        free_ast(ast);
        return NULL;
    }

    struct token *tok = pop(lex);
    *eof = (tok->token_type == END);
    free_token(tok);

    free_lex(lex);
    return ast;
}
