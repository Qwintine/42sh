#include "token.h"

#include <stdlib.h>

struct lex *init_lex(FILE *entry)
{
    struct lex *lex = malloc(sizeof(struct lex));
    if(!lex)
	    return NULL;
    lex->entry = entry;
    lex->current_token = NULL;
    lex->context = KEYWORD;
    lex->error = 0;
    return lex;
}

struct token *init_token(enum type context)
{
    struct token *tok = malloc(sizeof(struct token));
    if(!tok)
	    return NULL;
    tok->token_type = context;
    tok->value = calloc(1, sizeof(char));
    if(!tok->value)
    {
	    free(tok);
	    return NULL;
    }
    return tok;
}

void free_lex(struct lex *lex)
{
    if (lex->current_token)
        free_token(lex->current_token);
    free(lex);
}

void free_token(struct token *tok)
{
    if (tok->value)
        free(tok->value);
    free(tok);
}
