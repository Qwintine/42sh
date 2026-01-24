#include <stdlib.h>
#include <string.h>

#include "parser_aux.h"

/*
 * Description:
 * 	Parse an elif block when called
 * Verbose:
 * 	Grammar:
 * 		elif' compound_list 'then' compound_list [else_clause]
 */
struct ast *parser_elif(struct lex *lex, struct dictionnary *dict)
{
    if (!peek(lex) || peek(lex)->token_type != ELIF)
        return NULL;
    discard_token(pop(lex));

    struct ast_if *ast_if = (struct ast_if *)init_ast_if();

    ast_if->condition = parser_compound_list(lex, dict);

    if (!peek(lex) || peek(lex)->token_type != THEN || !ast_if->condition)
    {
        goto ERROR;
    }
    discard_token(pop(lex));

    ast_if->then_body = parser_compound_list(lex, dict);
    if (!ast_if->then_body)
    {
        goto ERROR;
    }

    if (peek(lex)
        && (peek(lex)->token_type == ELSE || peek(lex)->token_type == ELIF))
    {
        ast_if->else_body = parser_else_clause(lex, dict);
        if (!ast_if->else_body)
        {
            goto ERROR;
        }
    }

    return (struct ast *)ast_if;
ERROR:
    free_ast((struct ast *)ast_if);
    return NULL;
}

/*
 * Description:
 * 	Define which parser to use depending on the context
 * Verbose:
 * 	Grammar:
 * 		else_clause =
 * 			'else' compound_list
 * 		      | 'elif' compound_list 'then' compound_list [else_clause]
 * 		      ;
 */
struct ast *parser_else_clause(struct lex *lex, struct dictionnary *dict)
{
    if (!peek(lex) || peek(lex)->token_type == END)
        return NULL;

    if (peek(lex)->token_type == ELIF)
    {
        return parser_elif(lex, dict);
    }

    if (peek(lex)->token_type == ELSE)
    {
        discard_token(pop(lex));
        return parser_compound_list(lex, dict);
    }

    return NULL;
}

/*
 * Description:
 * 	Handle a 'if' block by calling corresponding parser at each step
 * Return:
 *
 * Verbose:
 * 	Grammar:
 * 		'if' compound_list 'then' compound_list [else_clause] 'fi' ;
 */
struct ast *parser_rule_if(struct lex *lex, struct dictionnary *dict)
{
    if (!peek(lex) || peek(lex)->token_type != IF) // si autre keyword
        return NULL;
    discard_token(pop(lex)); // consomme if

    struct ast_if *ast_if = (struct ast_if *)init_ast_if();

    ast_if->condition = parser_compound_list(lex, dict);
    if (!ast_if->condition)
    {
        goto ERROR;
    }

    if (!peek(lex) || peek(lex)->token_type != THEN)
    {
        goto ERROR;
    }
    discard_token(pop(lex));

    ast_if->then_body = parser_compound_list(lex, dict);
    if (!ast_if->then_body)
    {
        goto ERROR;
    }

    if (peek(lex)
        && (peek(lex)->token_type == ELSE || peek(lex)->token_type == ELIF))
    {
        ast_if->else_body = parser_else_clause(lex, dict);
        if (!ast_if->else_body)
        {
            goto ERROR;
        }
    }

    if (!peek(lex) || peek(lex)->token_type != FI)
    {
        goto ERROR;
    }
    discard_token(pop(lex));

    return (struct ast *)ast_if;

ERROR:
    free_ast((struct ast *)ast_if);
    return NULL;
}

/*
 * Description:
 * 	Handle a 'while' block by calling corresponding parser at each step
 * Return:
 *
 * Verbose:
 * 	Grammar:
 * 		'while' compound_list 'do' compound_list 'done' ;
 */
struct ast *parser_rule_while(struct lex *lex, struct dictionnary *dict)
{
    if (!peek(lex) || peek(lex)->token_type != WHILE)
        return NULL;
    discard_token(pop(lex));

    struct ast_loop *ast_loop = (struct ast_loop *)init_ast_loop();

    ast_loop->condition = parser_compound_list(lex, dict);
    if (!ast_loop->condition)
    {
        goto ERROR;
    }

    if (!peek(lex) || peek(lex)->token_type != DO)
    {
        goto ERROR;
    }
    discard_token(pop(lex));

    ast_loop->body = parser_compound_list(lex, dict);
    if (!ast_loop->body)
    {
        goto ERROR;
    }

    if (!peek(lex) || peek(lex)->token_type != DONE)
    {
        goto ERROR;
    }
    discard_token(pop(lex));

    return (struct ast *)ast_loop;

ERROR:
    free_ast((struct ast *)ast_loop);
    return NULL;
}

/*
 * Description:
 * 	Handle a 'until' block by calling corresponding parser at each step
 * Return:
 *
 * Verbose:
 * 	Grammar:
 * 		'until' compound_list 'do' compound_list 'done' ;
 */
struct ast *parser_rule_until(struct lex *lex, struct dictionnary *dict)
{
    if (!peek(lex) || peek(lex)->token_type != UNTIL)
        return NULL;
    discard_token(pop(lex));

    struct ast_loop *ast_loop = (struct ast_loop *)init_ast_loop();

    // inverse la condition
    ast_loop->truth = 1;

    ast_loop->condition = parser_compound_list(lex, dict);
    if (!ast_loop->condition)
    {
        goto ERROR;
    }

    if (!peek(lex) || peek(lex)->token_type != DO)
    {
        goto ERROR;
    }
    discard_token(pop(lex));

    ast_loop->body = parser_compound_list(lex, dict);
    if (!ast_loop->body)
    {
        goto ERROR;
    }

    if (!peek(lex) || peek(lex)->token_type != DONE)
    {
        goto ERROR;
    }
    discard_token(pop(lex));

    return (struct ast *)ast_loop;

ERROR:
    free_ast((struct ast *)ast_loop);
    return NULL;
}

static int parser_rule_for_aux(struct lex *lex, struct ast_for *ast_for)
{
    if (peek(lex)
        && (peek(lex)->token_type == IN || peek(lex)->token_type == NEWLINE))
    {
        while (peek(lex) && peek(lex)->token_type == NEWLINE)
            discard_token(pop(lex));
        if (!peek(lex) || peek(lex)->token_type != IN)
            return 1;
        discard_token(pop(lex));

        size_t w = 0;
        while (peek(lex) && (peek(lex)->token_type == WORD))
        {
            struct token *tok = pop(lex);
            if (!tok)
                return 1;

            ast_for->words[w] = tok->value;
            w++;
            char **new_words =
                realloc(ast_for->words, (w + 1) * sizeof(char *));
            if (!new_words)
            {
                free(tok);
                return 1;
            }
            ast_for->words = new_words;
            ast_for->words[w] = NULL;
            free(tok);
        }
    }
    return 0;
}

struct ast *parser_rule_for(struct lex *lex, struct dictionnary *dict)
{
    if (!peek(lex) || peek(lex)->token_type != FOR)
        return NULL;
    discard_token(pop(lex));

    struct ast_for *ast_for = (struct ast_for *)init_ast_for();

    if (!peek(lex) || (peek(lex)->token_type != WORD))
        goto ERROR;

    struct token *tok = pop(lex);
    if (!tok)
        goto ERROR;

    ast_for->var = tok->value;
    free(tok);

    if (parser_rule_for_aux(lex, ast_for))
        goto ERROR;

    if (peek(lex) && peek(lex)->token_type == SEMI_COLON)
        discard_token(pop(lex));

    while (peek(lex) && peek(lex)->token_type == NEWLINE)
        discard_token(pop(lex));

    if (!peek(lex) || peek(lex)->token_type != DO)
        goto ERROR;
    discard_token(pop(lex));

    ast_for->body = parser_compound_list(lex, dict);

    if (!peek(lex) || peek(lex)->token_type != DONE)
        goto ERROR;
    discard_token(pop(lex));

    return (struct ast *)ast_for;

ERROR:
    free_ast((struct ast *)ast_for);
    return NULL;
}

struct ast *parser_rule_command_block(struct lex *lex, struct dictionnary *dict)
{
    if (!peek(lex) || !(peek(lex)->token_type == OPENING_BRACKET))
        return NULL;
    discard_token(pop(lex));

    struct ast *ast = parser_compound_list(lex, dict);

    if (!peek(lex) || !(peek(lex)->token_type == CLOSING_BRACKET))
    {
        if (ast)
            free_ast(ast);
        return NULL;
    }
    discard_token(pop(lex));

    return ast;
}