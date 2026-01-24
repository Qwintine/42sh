#include <stdlib.h>

#include "parser_aux.h"

/*
 * Description:
 * 	Absorb a chain of and_or separeted by ';' or '\n'
 * Return:
 * 	*ast -> chained list of  parser_and_or result ast
 * Verbose:
 * 	Grammar:
 * 		{'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
 */
struct ast *parser_compound_list(struct lex *lex, struct dictionnary *dict)
{
    if (!peek(lex) || peek(lex)->token_type == END)
    {
        return NULL;
    }
    while (peek(lex) && peek(lex)->token_type == NEWLINE)
    {
        discard_token(pop(lex));
    }

    if (!peek(lex) || peek(lex)->token_type == THEN
        || peek(lex)->token_type == ELSE || peek(lex)->token_type == ELIF
        || peek(lex)->token_type == FI || peek(lex)->token_type == DO
        || peek(lex)->token_type == DONE || peek(lex)->token_type == END
        || peek(lex)->token_type == SEMI_COLON)
    {
        return NULL;
    }

    struct ast_list *head = (struct ast_list *)init_ast_list();
    struct ast_list *current = head;

    current->elt = parser_and_or(lex, dict);
    if (!current->elt)
    {
        free_ast((struct ast *)current);
        return NULL;
    }

    while (peek(lex)
           && (peek(lex)->token_type == SEMI_COLON
               || peek(lex)->token_type == NEWLINE))
    {
        discard_token(pop(lex));

        while (peek(lex) && peek(lex)->token_type == NEWLINE)
        {
            discard_token(pop(lex));
        }

        if (!peek(lex) || peek(lex)->token_type == END
            || peek(lex)->token_type == THEN || peek(lex)->token_type == ELSE
            || peek(lex)->token_type == ELIF || peek(lex)->token_type == FI
            || peek(lex)->token_type == DO || peek(lex)->token_type == DONE)
        {
            break;
        }

        if (peek(lex) && peek(lex)->token_type == SEMI_COLON)
        {
            free_ast((struct ast *)head);
            return NULL;
        }

        struct ast_list *new_node = (struct ast_list *)init_ast_list();
        new_node->elt = parser_and_or(lex, dict);
        if (!new_node->elt)
        {
            free_ast((struct ast *)new_node);
            return NULL;
        }
        current->next = new_node;
        current = new_node;
    }
    return (struct ast *)head;
}

/*
 * Description:
 * 	Parse list block into sub and_or block(s) separated by semi-colons
 * Arguments:
 * 	*lex -> struct of the lexer:
 * 		entry -> input stream
 * 		current_token -> last token returned by lexer
 * 		context -> context of the parser when retrieving a token
 * Return:
 * 	*ast:
 * 		NULL: Grammar/Syntax error
 * 		| *ast: ast of a list ( cf. Verbose )
 * Verbose:
 * 	Grammar:
 *		list = and_or { ';' and_or } [ ';' ]
 *	TODO
 */
struct ast *parser_list(struct lex *lex, struct dictionnary *dict)
{
    if (!peek(lex)
        || peek(lex)->token_type == END) // can't start with EOF -> Error
        return NULL;

    lex->context =
        KEYWORD; // list starts with keyword (command, operator, if, etc...)
    struct ast_list *head = (struct ast_list *)init_ast_list();
    struct ast_list *current = head;

    // récursion sur ast type and_or ( cf. parser_and_or )
    current->elt = parser_and_or(lex, dict); 
    if (!current->elt)
    {
        free(head);
        return NULL;
    }

    while (peek(lex) && (peek(lex)->token_type == SEMI_COLON)) // séparateurs
    {
        discard_token(pop(lex));

        if (!peek(lex) || peek(lex)->token_type == END)
            break;

        if (peek(lex) && peek(lex)->token_type == SEMI_COLON)
        {
            free_ast((struct ast *)head);
            return NULL;
        }

        struct ast_list *new_node = (struct ast_list *)
            init_ast_list(); // éléments liste de block de and_or
        new_node->elt = parser_and_or(lex, dict); // récursion sur ast type and_or
        if (!new_node->elt)
        {
            free(new_node);
            break;
        }
        current->next = new_node; // liste de and_or
        current = new_node;
    }
    return (struct ast *)head; // 1er elem liste
}
