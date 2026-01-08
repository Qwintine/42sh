#include "token.h"

struct lex *init_lex(FILE *entry)
{
    struct lex *lex = malloc(sizeof(struct lex));
    lex->entry = entry;
    lex->current_token = NULL;
    lex->context = WORD;
    return lex;
}

struct token *init_token(enum type context)
{
    struct token *tok = malloc(sizeof(struct token));
    tok->token_type = context;
    tok->value = calloc(1, sizeof(char));
    return tok;    
}

void free_lex(struct lex *lex)
{
    if (lex->current_token)
        free_token(lex->current_token);
    fclose(lex->entry);
    free(lex);
}

void free_token(struct token *tok)
{
    if (tok->value)
        free(tok->value);
    free(tok);
}
