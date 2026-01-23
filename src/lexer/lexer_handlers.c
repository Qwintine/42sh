#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>

#include "lexer_aux.h"

/*
 * Description:
 *	Helper;  Handle different \ behaviors
 * Arguments:
 *	char **value -> value to concat to if needed
 *	FILE *entry -> Input stream
 *	int in_quotes -> quote state
 * Return:
 *	0 success / 1 failure
 */

int handle_com(int in_quotes, struct lex *lex, struct token *tok, char *buf)
{
    if (!in_quotes)
    {
        if (!(strlen(tok->value) > 0))
        {
            while (fread(buf, 1, 1, lex->entry))
                if (buf[0] == '\n')
                    break;
            if (buf[0] == '\n')
                fseek(lex->entry, -1, SEEK_CUR);
        }
        else
            tok->value = concat(tok->value, '#');
        if (!tok->value)
            return 1;
    }
    else
    {
        tok->value = concat(tok->value, '#');
        if (!tok->value)
            return 1;
    }
    return 0;
}

int handle_backslash(char **value, FILE *entry)
{
    char buf[1];
    if (!fread(buf, 1, 1, entry))
    {
        *value = concat(*value, '\\');
        if (!*value)
            return 1;
        return 0;
    }

    if (buf[0] == '$' || buf[0] == '`' || buf[0] == '"' || buf[0] == '\\'
        || buf[0] == '\n')
    {
        *value = concat(*value, '\\');
        if (buf[0] != '\n')
        {
            *value = concat(*value, buf[0]);
            if (!*value)
                return 1;
        }
    }
    else
    {
        *value = concat(*value, '\\');
        if (!*value)
            return 1;
        *value = concat(*value, buf[0]);
        if (!*value)
            return 1;
    }
    return 0;
}

int handle_quote(int *quote, int other_quote, struct token *tok, char val)
{
    tok->value = concat(tok->value, val);
    if (!other_quote)
    {
        *quote = !(*quote);
        if (!(*quote) && strlen(tok->value) == 0)
            return 1;
    }
    return 0;
}

int handle_newline(struct token *tok, int quote, FILE *entry)
{
    if (!quote)
    {
        if (strlen(tok->value) > 0)
        {
            fseek(entry, -1, SEEK_CUR);
            return 1;
        }
        tok->value = concat(tok->value, '\n');
        if (!tok->value
            || (tok->token_type != WORD && tok->token_type != KEYWORD))
            return -1;
        tok->token_type = NEWLINE;
        return 1;
    }
    tok->value = concat(tok->value, '\n');
    if (!tok->value)
        return -1;
    return 0;
}

int handle_semicolon(struct token *tok, int quote, FILE *entry)
{
    if (!quote)
    {
        if (strlen(tok->value) > 0)
        {
            fseek(entry, -1, SEEK_CUR);
            return 1;
        }
        tok->value = concat(tok->value, ';');
        if (!tok->value
            || (tok->token_type != WORD && tok->token_type != KEYWORD))
            return -1;
        tok->token_type = SEMI_COLON;
        return 1;
    }
    tok->value = concat(tok->value, ';');
    if (!tok->value)
        return -1;
    return 0;
}

int handle_blank(struct token *tok, char c, int quote)
{
    if (!quote)
    {
        if (strlen(tok->value) > 0)
            return 1;
        return 0;
    }
    tok->value = concat(tok->value, c);
    if (!tok->value)
        return -1;
    return 0;
}

int handle_delimiter(struct token *tok, char c, int quote, FILE *entry)
{
    if (c == '\n')
        return handle_newline(tok, quote, entry);
    if (c == ';')
        return handle_semicolon(tok, quote, entry);
    return handle_blank(tok, c, quote);
}

int new_op(struct token *tok, int quote, FILE *entry, char val)
{
    if (!quote)
    {
        if (strlen(tok->value) > 0)
        {
            fseek(entry, -1, SEEK_CUR);
            if (val == '>' || val == '<')
                if (valid_io(tok->value))
                    tok->token_type = IO_NUMBER;
            return 1;
        }
        tok->value = concat(tok->value, val);
        if (!tok->value
            || (tok->token_type != WORD && tok->token_type != KEYWORD))
            return -1;
        return 0;
    }
    tok->value = concat(tok->value, val);
    if (!tok->value)
        return -1;
    return 0;
}

static int handle_opening_bracket(struct lex *lex, struct token *tok,
                           struct quote_status *quote_status)
{
    if (!quote_status->double_quote && !quote_status->single_quote)
    {
        if (tok->value && tok->value[0])
        {
            if (tok->value[0] == '$')
            {
                tok->value = concat(tok->value, '{');
                if (!tok->value)
                    return 1;
                tok->token_type = WORD;
                quote_status->bracket_open = 1;
                return -1;
            }
            else
            {
                return 1;
            }
        }
        else if (lex->context == KEYWORD)
        {
            tok->value = concat(tok->value, '{');
            if (!tok->value)
                return 1;
            tok->token_type = OPENING_BRACKET;
            lex->current_token = tok;
            return 0;
        }
        else
        {
            tok->value = concat(tok->value, '{');
            if (!tok->value)
                return 1;
            tok->token_type = WORD;
        }
    }
    else
    {
        tok->value = concat(tok->value, '{');
        if (!tok->value)
            return 1;
    }
    return -1;
}

static int handle_closing_bracket(struct lex *lex, struct token *tok,
                           struct quote_status *quote_status)
{
    if (!quote_status->double_quote && !quote_status->single_quote)
    {
        if (quote_status->bracket_open)
        {
            tok->value = concat(tok->value, '}');
            if (!tok->value)
                return 1;
            tok->token_type = WORD;
            quote_status->bracket_open = 0;
            return -1;
        }
        else if (tok->value && tok->value[0])
        {
            return 1;
        }
        else if (lex->context == KEYWORD)
        {
            tok->value = concat(tok->value, '}');
            if (!tok->value)
                return 1;
            tok->token_type = CLOSING_BRACKET;
            lex->current_token = tok;
            return 0;
        }
        else
        {
            tok->value = concat(tok->value, '}');
            if (!tok->value)
                return 1;
            tok->token_type = WORD;
        }
    }
    else
    {
        tok->value = concat(tok->value, '}');
        if (!tok->value)
            return 1;
    }
    return -1;
}

int handle_bracket(struct lex *lex, struct token *tok,
                    struct quote_status *quote_status, char val)
{
    if (val == '{')
        return handle_opening_bracket(lex, tok, quote_status);
    return handle_closing_bracket(lex, tok, quote_status);
}