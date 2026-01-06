#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/token.h"
#include "lexer.h"

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

static int handle_backslash(char **value, FILE *entry, int in_quotes)
{
	char buf[1];
	if (in_quotes)
	{
		*value = concat(*value, '\\');
		if (!*value)
			return 0;
	}
	else
	{
		if (!fread(buf, 1, 1, entry))
			return 0;
		if (buf[0] != '\n')
		{
			*value = concat(*value, '\\');
			if (!*value)
				return 0;
			*value = concat(*value, buf[0]);
			if (!*value)
				return 0;
		}
	}
	return 1;
}

static int handle_quote(char **value, char c, int *quote, int other_quote)
{
	if (!other_quote)
		*quote = !(*quote);
	*value = concat(*value, c);
	if (!*value)
		return 0;
	return 1;
}

static int handle_newline(struct token *tok, int quote)
{
	if (!quote)
	{
		if (strlen(tok->value) > 0)
			return 1;
		tok->token_type = NEWLINE;
		return 1;
	}
	tok->value = concat(tok->value, '\n');
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

static int handle_delimiter(struct token *tok, char c, int quote)
{
	if (c == '\n')
		return handle_newline(tok, quote);
	return handle_blank(tok, c, quote);
}

static struct token *end_token(struct token *tok, FILE *entry)
{
	if (feof(entry) != 0)
	{
		if (strlen(tok->value) == 0)
			tok->token_type = END;
		return tok;
	}
	free(tok->value);
	free(tok);
	return NULL;
}

/* Description:
 * 	transforme le FILE en token
 * Arguments:
 * 	le FILE à parser
 * Retour:
 * 	struct token * -> token générés
 * Verbose:
 * 	Suit la SCL pour créer les tokens
 */
struct token *lexer(FILE *entry)
{
	int double_quote = 0;
	int single_quote = 0;
	char buf[1];
	struct token *tok = malloc(sizeof(struct token));
	if(!tok)
	{
		return NULL;
	}
	tok->value = calloc(1,1);
	tok->token_type = WORD;
	while(fread(buf, 1, 1, entry))
	{
		switch (buf[0])
		{
			case '"':	// cas 4
				if (!handle_quote(&tok->value, buf[0], &double_quote, single_quote))
					goto ERROR;
				break;
			case '\'':	// cas 4
				if (!handle_quote(&tok->value, buf[0], &single_quote, double_quote))
					goto ERROR;
				break;
			case '\\': // cas 4
				if (!handle_backslash(&tok->value, entry, double_quote || single_quote))
					goto ERROR;
				break;
			case '\n':	// cas 7
			case ' ':	// cas 8
			case '\t':
			{
				int result = handle_delimiter(tok, buf[0], double_quote || single_quote);
				if (result < 0)
					goto ERROR;
				if (result > 0)
					return tok;
				break;
			}
			default: 	// cas 9 et 11
				tok->value = concat(tok->value, buf[0]);
				if(!tok->value)
					goto ERROR;
		}

	}
	return end_token(tok, entry); //cas 1
	ERROR:
			free(tok->value);
			free(tok);
			return NULL;
}

/* Main de test (temporaire)
#include "../io/io.h"
int main()
{
	FILE *f = arg_file(3, (char*[]){"program", "-c", "Hello 'W  o'   \n   \\n 'r   ld'     !"});
	struct token *t = lexer(f);
	while (t->token_type != END)
	{
		printf("Token: %s (type: %d)\n", t->value, t->token_type);
		free(t->value);
		free(t);
		t = lexer(f);
	}
	printf("Token: %s (type: %d)\n", t->value, t->token_type);
	free(t->value);
	free(t);
	fclose(f);
	return 0;
}
*/