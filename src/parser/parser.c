#include <stdio.h>
#include "parser.h"
#include "../lexer/lexer.h"
#include "../utils/token.h"

/*
 * Descriptif a faire
 */
static struct token *peek(struct lex *lex)
{
	if(lex)
	{
		if(!lex->current_token)
		{
			int res = lexer(lex);
			if (res)
				return NULL;
		}
		return lex->current_token;
	}
	return NULL;
}

/*
 * Descriptif a faire
 */
static struct token *pop(struct lex *lex)
{
	if(lex)
	{
		struct token *tok = lex->current_token;
		lex->current_token = NULL;
		return tok;
	}
	return NULL;
}

// pas sur que soit bien qu'on appelle directement le parser
static struct ast *parser_list(struct lex *lex)
{
	struct ast_list *ast_list = (struct ast_list *)init_ast_list();
	ast_list->elt = parser_and_or(lex);
	if (peek(lex) && (peek(lex)->token_type == SEMI_COLON || peek(lex)->token_type == NEWLINE))
	{
		free_token(pop(lex));
		ast_list->next = (struct ast_list *)parser_list(lex);
	}
	return (struct ast *)ast_list;
}

struct ast *parser(FILE *entry)
{
	struct lex *lex = init_lex(entry);
	if (!peek(lex))
	{
		free_lex(lex);
		return NULL;
	}
	if (peek(lex) && (peek(lex)->token_type == NEWLINE || peek(lex)->token_type == END))
	{
		free_lex(lex);
		return init_ast_list();
	}

	struct ast *ast = parser_list(lex);
	struct token *tok = pop(lex);
	
	if (!tok || (tok->token_type != NEWLINE && tok->token_type != END))
	{
		if (tok)
			free_token(tok);
		free_lex(lex);
		free_ast(ast);
		return NULL;
	}
	free_token(tok);
	free_lex(lex);
	return ast;
}