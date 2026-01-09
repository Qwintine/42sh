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
		if(!lex->current_token)
		{
			peek(lex);
		}
		struct token *tok = lex->current_token;
		lex->current_token = NULL;
		lex->context = NULL;
		return tok;
	}
	return NULL;
}
/*
 * Description:
 * 		Free a discarded token
 * Arguments:
 * 		Token to free
 * Return:
 * 		1 Succes / 0 Error
 */
int discard_token(struct token *tok)
{
	if(!tok)
	{
		return 0;
	}
	free_tok(tok);
	return 1;
}

//============================ Parser Grammar =================================

static struct ast *parser_compound_list(struct lex *lex)
{
	while(peek(lex) && peek(lex)->token_type == NEWLINE)
	{
		discard_token(lex);
	}
	struct ast_list *ast_list = init_ast_list();
	ast_list->elt = parser_and_or(lex);
	if(peek(lex) && peek(lex)->token_type == SEMI_COLON)
	{
		discard_token(lex);
	}
	while(peek(lex) && peek(lex)->token_type == NEWLINE)
	{
		discard_token(lex);
	}
	//checker bug ici
	lex->context = COMMAND;
	ast_list->next = (struct ast_list *)parser_and_or(lex);
	return (struct ast *)ast_list;
}

static struct ast *parser_elif(struct lex *lex)
{
	lex->context = ELIF;
	if(!discard_token(lex))
		return NULL;
	struct ast_if *ast_if = (struct ast_if *)init_ast_if();
	ast_if->condition = parser_compound_list(lex);
	lex->context = THEN;
	if(!discard_token(lex))
	{
		free_ast_if(ast_if->base);
		return NULL;
	}
	ast_if->then_body = parser_compound_list(lex);
	lex->context = ELSE;
	if(discard_token(lex))
	{
		ast_if->else_body = parser_else_clause(lex);
	}
	return (struct ast *) ast_if;
}

static struct ast *parser_else_clause(struct lex *lex)
{
	if(peek(lex) && peek(lex)->token_type == ELSE)
	{
		discard_token(lex);
		return parser_compound_list(lex);
	}
	return parser_elif(lex);
}

static struct ast *parser_rule_if(struct lex *lex)
{
	lex->context = IF;
	if(!discard_token(lex))
		return NULL;
	struct ast_if *ast_if = (struct ast_if *)init_ast_if();
	ast_if->condition = parser_compound_list(lex);
	lex->context = THEN;
	if(!discard_token(lex))
	{
		free_ast_if(ast_if->base);
		return NULL;
	}
	ast_if->then_body = parser_compound_list(lex);
	lex->context = ELSE;
	if(discard_token(lex))
	{
		ast_if->else_body = parser_else_clause(lex);
	}
	lex->context = FI;
	if(!discard_token(lex))
	{
		free_ast_if(ast_if->base);
		return NULL;
	}
	return (struct ast *) ast_if;
}

static struct ast *parser_shell_command(struct lex *lex)
{
	return parser_rule_if(lex);
}

static struct ast *parser_simple_command(struct lex *lex)
{
	struct ast_cmd *ast_cmd = (struct ast_cmd *)init_ast_cmd();
	size_t ind = 0;
	lex->context = WORD;
	while(peek(lex) && peek(lex)->token_type == WORD)
	{
		struct token *tok = pop(lex);
		ast_cmd->words[ind] = tok->value;
		free_token(tok);
		ind++;
		ast_cmd->words = realloc(ast_cmd->words,ind+1);
		ast_cmd->words[ind] = NULL;
	}
	return (struct ast *)ast_cmd;
}

static struct ast *parser_command(struct lex *lex)
{
	if(peek(lex) && peek(lex)->token_type == IF)
	{
		return parser_shell(lex);
	}
	return parser_simple_command(lex);
}

static struct ast *parser_pipeline(struct lex *lex)
{
	return parser_command(lex);
}

static struct ast *parser_and_or(struct lex *lex)
{
	return parser_pipeline(lex);
}

// pas sur que soit bien qu'on appelle directement le parser
static struct ast *parser_list(struct lex *lex)
{
	struct ast_list *ast_list = (struct ast_list *)init_ast_list();
	ast_list->elt = parser_and_or(lex);
	if (peek(lex) && (peek(lex)->token_type == SEMI_COLON || peek(lex)->token_type == NEWLINE))
	{
		discard_token(lex);
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
