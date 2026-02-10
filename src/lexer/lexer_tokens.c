#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>

#include "lexer_aux.h"

/** Termine le token en cours si fin de fichier*/
struct token *end_token(struct token *tok, struct lex *lex)
{
    if (feof(lex->entry) != 0)
    {
        if (strlen(tok->value) == 0)
        {
            tok->token_type = END;
            if (lex->context != WORD && lex->context != KEYWORD)
                return NULL;
        }
        else if (strlen(tok->value) == 1)
        {
            if (tok->value[0] == '&')
                tok->token_type = SEMI_COLON;
            else if (tok->value[0] == '|')
                tok->token_type = PIPE;
            else if (tok->value[0] == '<')
                return NULL;
            else if (tok->value[0] == '>')
                return NULL;
        }
        return tok;
    }
    return NULL;
}

int verif_token_if(struct token *tok, enum type context)
{
    switch (context)
    {
    case IF:
        if (tok->value == NULL || strcmp(tok->value, "if") != 0)
            return 1;
        break;
    case THEN:
        if (tok->value == NULL || strcmp(tok->value, "then") != 0)
            return 1;
        break;
    case ELIF:
        if (tok->value == NULL || strcmp(tok->value, "elif") != 0)
            return 1;
        break;
    case ELSE:
        if (tok->value == NULL || strcmp(tok->value, "else") != 0)
            return 1;
        break;
    case FI:
        if (tok->value == NULL || strcmp(tok->value, "fi") != 0)
            return 1;
        break;
    default:
        break;
    }
    return 0;
}

int verif_token_loop(struct token *tok, enum type context)
{
    switch (context)
    {
    case WHILE:
        if (tok->value == NULL || strcmp(tok->value, "while") != 0)
            return 1;
        break;
    case UNTIL:
        if (tok->value == NULL || strcmp(tok->value, "until") != 0)
            return 1;
        break;
    case FOR:
        if (tok->value == NULL || strcmp(tok->value, "for") != 0)
            return 1;
        break;
    case DO:
        if (tok->value == NULL || strcmp(tok->value, "do") != 0)
            return 1;
        break;
    case DONE:
        if (tok->value == NULL || strcmp(tok->value, "done") != 0)
            return 1;
        break;
    case IN:
        if (tok->value == NULL || strcmp(tok->value, "in") != 0)
            return 1;
        break;
    default:
        break;
    }
    return 0;
}

/** Vérifie si le token correspond au type attendu*/
int verif_token(struct token *tok, enum type context)
{
    if (context == IF || context == THEN || context == ELIF || context == ELSE
        || context == FI)
    {
        return verif_token_if(tok, context);
    }
    else if (context == WHILE || context == UNTIL || context == FOR
             || context == DO || context == DONE || context == IN)
    {
        return verif_token_loop(tok, context);
    }
    return 0;
}

/** Retourne le type du token en fonction de sa valeur*/
enum type check_type(char *value)
{
    if (strcmp(value, "if") == 0)
        return IF;
    if (strcmp(value, "then") == 0)
        return THEN;
    if (strcmp(value, "elif") == 0)
        return ELIF;
    if (strcmp(value, "else") == 0)
        return ELSE;
    if (strcmp(value, "fi") == 0)
        return FI;
    if (strcmp(value, "!") == 0)
        return NEGATION;
    if (strcmp(value, "while") == 0)
        return WHILE;
    if (strcmp(value, "until") == 0)
        return UNTIL;
    if (strcmp(value, "for") == 0)
        return FOR;
    if (strcmp(value, "do") == 0)
        return DO;
    if (strcmp(value, "done") == 0)
        return DONE;
    if (strcmp(value, "in") == 0)
        return IN;
    return WORD;
}
