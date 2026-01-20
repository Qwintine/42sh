#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer_aux.h"

static int sub_switch_op(struct lex *lex, struct token *tok, char *buf,
                         struct quote_status *quote_status)
{
    switch (buf[0])
    {
    case '`':
    case '&': // case 6
    case '|':
    case '>':
    case '<': {
        int result =
            new_op(tok, quote_status->double_quote, lex->entry, buf[0]);
        if (result < 0)
            return 1;
        if (result > 0)
        {
            lex->current_token = tok;
            if (lex->current_token->token_type == KEYWORD
                && lex->context == KEYWORD)
                lex->current_token->token_type =
                    check_type(lex->current_token->value);
            return 0;
        }
        break;
    }
    default:
        tok->value = concat(tok->value, buf[0]);
        if (!tok->value)
            return 1;
    }
    return -1;
}

static int sub_switch_delim(struct lex *lex, struct token *tok, char *buf,
                            struct quote_status *quote_status)
{
    switch (buf[0])
    {
    case '\\': // cas 4
        if (handle_backslash(&tok->value, lex->entry,
                             quote_status->double_quote))
            return 1;
        break;
    case ';':
    case '\n': // cas 7
    case ' ': // cas 8
    case '\t': {
        int result = handle_delimiter(tok, buf[0],
                                      quote_status->double_quote
                                          || quote_status->single_quote,
                                      lex->entry);
        if (result < 0)
            return 1;
        if (result > 0)
        {
            lex->current_token = tok;
            if (lex->current_token->token_type == KEYWORD
                && lex->context == KEYWORD)
                lex->current_token->token_type =
                    check_type(lex->current_token->value);
            return 0;
        }
        break;
    }
    default: // cas 9 et 11
        return sub_switch_op(lex, tok, buf, quote_status);
    }
    return -1;
}

static int sub_switch(struct lex *lex, struct token *tok, char *buf,
                      struct quote_status *quote_status)
{
    if (quote_status->single_quote && buf[0] != '\'')
    {
        tok->value = concat(tok->value, buf[0]);
        if (!tok->value)
            return 1;
        return -1;
    }

    switch (buf[0])
    {
    case '#':
        if (handle_com(quote_status->double_quote || quote_status->single_quote,
                       lex, tok, buf))
            return 1;
        break;

    case '"': // cas 4
        if (handle_quote(&quote_status->double_quote,
                         quote_status->single_quote, tok, buf[0]))
        {
            lex->current_token = tok;
            tok->token_type = WORD;
            return 0;
        }
        break;
    case '\'': // cas 4
        if (handle_quote(&quote_status->single_quote,
                         quote_status->double_quote, tok, buf[0]))
        {
            lex->current_token = tok;
            tok->token_type = WORD;
            return 0;
        }
        break;
    case '=': {
        tok->value = concat(tok->value, '=');
        if (!tok->value)
            return 1;
        if (!quote_status->double_quote && !quote_status->single_quote
            && strlen(tok->value) > 1 && lex->context == KEYWORD)
        {
            tok->token_type = ASSIGNMENT;
        }
        break;
    }
    default:
        return sub_switch_delim(lex, tok, buf, quote_status);
    }
    return -1;
}

/* Description:
 * 	transforme le FILE en token
 * Arguments:
 * 	le FILE et le contexte à parser
 * Retour:
 * 	int -> code de retour (0 = succès, 1 = erreur)
 * Verbose:
 * 	Suit la SCL pour créer les tokens
 */
int lexer(struct lex *lex)
{
    struct quote_status quote_status = { 0, 0 };
    char buf[1];
    struct token *tok = init_token(lex->context);
    if (!tok)
        return 1;
    while (fread(buf, 1, 1, lex->entry))
    {
        if (tok->value && tok->value[0] && !quote_status.single_quote
            && !quote_status.double_quote
            && (tok->value[0] == '&' || tok->value[0] == '|'
                || tok->value[0] == '>' || tok->value[0] == '<')) // cas 2/3
        {
            int res;
            if (tok->value[0] == '&' || tok->value[0] == '|')
                res = manage_op(lex, tok, buf);
            else if (tok->value[0] == '>' || tok->value[0] == '<')
                res = manage_redir(lex, tok, buf);
            if (!res)
                return 0;
            goto ERROR;
        }
        else
        {
            int res = sub_switch(lex, tok, buf, &quote_status); // cas 2-11
            if (res == 0)
                return 0;
            if (res == 1)
                goto ERROR;
        }
    }
    lex->current_token = end_token(tok, lex); // cas 1
    if (!lex->current_token || verif_token(lex->current_token, lex->context)
        || quote_status.double_quote || quote_status.single_quote)
        goto ERROR;
    if (lex->current_token->token_type == KEYWORD && lex->context == KEYWORD)
        lex->current_token->token_type = check_type(lex->current_token->value);
    return 0;
ERROR:
    if (lex->current_token == tok)
        lex->current_token = NULL;
    fseek(lex->entry, -(strlen(tok->value)), SEEK_CUR);
    free_token(tok);
    return 1;
}
