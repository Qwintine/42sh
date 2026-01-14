#define _POSIX_C_SOURCE 200809L
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/token.h"

/*
 * Description:
 *	Helper; Concat char to token value
 * Arguments:
 *	char *val -> current token value
 *	char c -> char to concat
 * Return:
 *	New token value
 */
static char *concat(char *val, char c)
{
    size_t size = strlen(val);
    char *new_val = realloc(val, size + 2);
    if (!new_val)
        return NULL;
    new_val[size] = c;
    new_val[size + 1] = '\0';
    return new_val;
}

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

static int handle_com(int in_quotes, struct lex *lex, struct token *tok,
                      char *buf)
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

static int handle_backslash(char **value, FILE *entry, int in_quotes)
{
    char buf[1];
    if (in_quotes)
    {
        *value = concat(*value, '\\');
        if (!*value)
            return 1;
    }
    else
    {
        if (!fread(buf, 1, 1, entry))
        {
            *value = concat(*value, '\\');
            if (!*value)
                return 1;
            return 0;
        }
        if (buf[0] != '\n')
        {
            *value = concat(*value, buf[0]);
            if (!*value)
                return 1;
        }
    }
    return 0;
}

static int handle_quote(int *quote, int other_quote, struct token *tok)
{
    if (!other_quote)
    {
        *quote = !(*quote);
        if (!(*quote) && strlen(tok->value) == 0)
            return 1;
    }
    return 0;
}

static int handle_newline(struct token *tok, int quote, FILE *entry)
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

static int handle_semicolon(struct token *tok, int quote, FILE *entry)
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

static int handle_blank(struct token *tok, char c, int quote)
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

static int handle_delimiter(struct token *tok, char c, int quote, FILE *entry)
{
    if (c == '\n')
        return handle_newline(tok, quote, entry);
    if (c == ';')
        return handle_semicolon(tok, quote, entry);
    return handle_blank(tok, c, quote);
}

static struct token *end_token(struct token *tok, struct lex *lex)
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
        }
        return tok;
    }
    return NULL;
}

static int verif_token_if(struct token *tok, enum type context)
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

static int verif_token_loop(struct token *tok, enum type context)
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

static int verif_token(struct token *tok, enum type context)
{
    if (context == IF || context == THEN || context == ELIF
        || context == ELSE || context == FI)
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
    if(strcmp(value, "!") == 0)
	    return NEGATION;
    if(strcmp(value, "while") == 0)
        return WHILE;
    if(strcmp(value, "until") == 0)
        return UNTIL;
    if(strcmp(value, "for") == 0)
        return FOR;
    if(strcmp(value, "do") == 0)
        return DO;
    if(strcmp(value, "done") == 0)
        return DONE;
    if(strcmp(value, "in") == 0)
        return IN;
    return WORD;
}

static int new_op(struct token *tok, int quote, FILE *entry, char val)
{
    if (!quote)
    {
        if (strlen(tok->value) > 0)
        {
            fseek(entry, -1, SEEK_CUR);
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

static int sub_switch_delim(struct lex *lex, struct token *tok, char *buf,
                            struct quote_status *quote_status)
{
    switch (buf[0])
    {
    case '\\': // cas 4
        if (handle_backslash(&tok->value, lex->entry,
                             quote_status->double_quote
                                 || quote_status->single_quote))
            return 1;
        break;
    case '$': // case 5
    case '`':
	//int result = handle_expansion(); //TODO
    case '&': // case 6
    case '|':
    {
        int result = new_op(tok, (quote_status->double_quote
                                    || quote_status->single_quote), lex->entry,
                    buf[0]);
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
        tok->value = concat(tok->value, buf[0]);
        if (!tok->value)
            return 1;
    }
    return -1;
}

static int sub_switch(struct lex *lex, struct token *tok, char *buf,
                      struct quote_status *quote_status)
{
    switch (buf[0])
    {
    case '#':
        if (handle_com(quote_status->double_quote || quote_status->single_quote,
                       lex, tok, buf))
            return 1;
        break;

    case '"': // cas 4
        if (handle_quote(&quote_status->double_quote,
                         quote_status->single_quote, tok))
        {
            lex->current_token = tok;
            if (tok->token_type == KEYWORD && lex->context == KEYWORD)
                tok->token_type = check_type(tok->value);
            return 0;
        }
        break;
    case '\'': // cas 4
        if (handle_quote(&quote_status->single_quote,
                         quote_status->double_quote, tok))
        {
            lex->current_token = tok;
            if (tok->token_type == KEYWORD && lex->context == KEYWORD)
                tok->token_type = check_type(tok->value);
            return 0;
        }
        break;
    default:
        return sub_switch_delim(lex, tok, buf, quote_status);
    }
    return -1;
}

/*
 * Description:
 * 	Handle a possible operator
 * Argument:
 * 	lex -> struct lexer
 * 	tok -> token being created
 * 	buf[] -> current character
 * 	quote_status -> if in quotes
 * Return:
 * 	0 Token created / 1 Error
 */
static int manage_op(struct lex *lex, struct token *tok, char buf[])
{
	if(tok->value[strlen(tok->value)-1] == '&')
	{
		if(buf[0] == '&')
		{
			concat(tok->value, '&');
			if(!tok->value)
			{
				return 1;
			}
			tok->token_type = AND;
		}
		else
		{
			tok->token_type = SEMI_COLON;
			fseek(lex->entry, -1, SEEK_CUR);
		}
	}
	else
	{
		if(buf[0] == '|')
		{
			concat(tok->value, '|');
			if(!tok->value)
			{
				return 1;
			}
			tok->token_type = OR;
		}
		else
		{
			tok->token_type = PIPE;
			fseek(lex->entry, -1, SEEK_CUR);
		}
	}
	lex->current_token = tok;
	return 0;
}

// Upgrade/Quality of life: Quote status from enum instead of int

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
	    //debut changement -> peut etre implem une size lors creation token 
	    if(tok->value && tok->value[0]
	    && !quote_status.single_quote && !quote_status.double_quote
	    && (tok->value[0] == '&' || tok->value[0] == '|'))
	    {
		    int res = manage_op(lex, tok, buf); // cas 2/3
		    if(!res)
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
