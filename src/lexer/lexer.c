#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/token.h"
#include "lexer.h"

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
			return 1;
		if (buf[0] != '\n')
		{
			*value = concat(*value, '\\');
			if (!*value)
				return 1;
			*value = concat(*value, buf[0]);
			if (!*value)
				return 1;
		}
	}
	return 0;
}


static int handle_quote(int *quote, int other_quote)
{
	if (!other_quote)
		*quote = !(*quote);
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
		if (!tok->value || (tok->token_type != WORD && tok->token_type != COMMAND))
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
		if (!tok->value || (tok->token_type != WORD && tok->token_type != COMMAND))
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
			if (lex->context != WORD && lex->context != COMMAND)
				return NULL;
		}
		return tok;
	}
	return NULL;
}

static int verif_token(struct token *tok, enum type context)
{
	switch (context)
	{
		case IF:
			if (tok->value == NULL || strcmp(tok->value, "if") != 0)
				return 1;
			break;
		case THEN:
			if (tok->value == NULL || strcmp(tok->value, "then") !=	 0)
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
	return WORD;
}

//Upgrade/Quality of life: Quote status from enum instead of int

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
	int double_quote = 0;
	int single_quote = 0;
	int com = 0;
	char buf[1];
	struct token *tok = init_token(lex->context);
	if(!tok)
		return 1;
	while(fread(buf, 1, 1, lex->entry))
	{
		switch (buf[0])
		{
			case '#':
				if (!double_quote && !single_quote)
				{
					while (fread(buf, 1, 1, lex->entry))
						if (buf[0] == '\n')
							break;
					if (buf[0] == '\n')
						fseek(lex->entry, -1, SEEK_CUR);
				}
				else
				{
					tok->value = concat(tok->value, '#');
					if(!tok->value)
						goto ERROR;
				}
				break;
			case '"':	// cas 4
				if (handle_quote(&double_quote, single_quote))
					goto ERROR;
				break;
			case '\'':	// cas 4
				if (handle_quote(&single_quote, double_quote))
					goto ERROR;
				break;
			case '\\': // cas 4
				if (handle_backslash(&tok->value, lex->entry, double_quote || single_quote))
					goto ERROR;
				break;
			case ';':
			case '\n':	// cas 7
			case ' ':	// cas 8
			case '\t':
			{
				int result = handle_delimiter(tok, buf[0], double_quote || single_quote, lex->entry);
				if (result < 0)
					goto ERROR;
				if (result > 0)
				{
					lex->current_token = tok;
					return 0;
				}
				break;
			}
			default: 	// cas 9 et 11
				tok->value = concat(tok->value, buf[0]);
				if(!tok->value)
					goto ERROR;
		}
	}
	lex->current_token = end_token(tok, lex); //cas 1
	if (!lex->current_token)
		goto ERROR;
	if (verif_token(lex->current_token, lex->context))
		goto ERROR;
	if (lex->current_token->token_type == COMMAND)
		lex->current_token->token_type = check_type(lex->current_token->value);
	return 0;
	ERROR:
		fseek(lex->entry, -(strlen(tok->value)), SEEK_CUR);
		free_token(tok);
		return 1;
}

