#include <stdio.h>
#include "../utils/token.h"
#include "lexer.h"

static void concat(char *val, char *c)
{
	size_t size = 1;
	while(val[size - 1] !=0)
		size++;
	val = realloc(val,size+1);
	val[size] = 0;
	val[size-1] = *c;
}

//faire description
struct token *lexer(FILE *entry)
{
	char *buf[1];
	struct token tok = malloc(sizeof(struct token));
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
			case '\n':
				concat(tok->val,buf);
				if(!tok->val)
					goto ERROR;
				return tok;
			case ' ':
				return tok;
			default:
				concat(tok->value,buf);
				if(!tok-val)
					goto ERROR;
		}

	}
	if(feof(entry) != 0)
	{
		if(!tok->value)
		{
			tok->token_type = END;
		}
		return tok;
	}
	else
	{
		goto ERROR;
	}
	ERROR:
			free(tok);
			return NULL;
}


