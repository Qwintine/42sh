#include <stdio.h>
#include <stdlib.h>
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
	free_token(tok);
	return 1;
}

//============================ Parser Grammar =================================

static struct ast *parser_and_or(struct lex *lex);
static struct ast *parser_else_clause(struct lex *lex);
static struct ast *parser_list(struct lex *lex);


static struct ast *parser_compound_list(struct lex *lex)
{
	if(!peek(lex) || peek(lex)->token_type == END)
	{
		return NULL;
	}
	while(peek(lex) && peek(lex)->token_type == NEWLINE)
	{
		discard_token(pop(lex));
	}
	
	struct ast_list *ast_list = (struct ast_list *)init_ast_list();
	ast_list->elt = parser_and_or(lex);
	
	struct ast_list *current = ast_list;
	while(peek(lex) && (peek(lex)->token_type == SEMI_COLON || peek(lex)->token_type == NEWLINE))
	{
		discard_token(pop(lex));
		
		while(peek(lex) && peek(lex)->token_type == NEWLINE)
		{
			discard_token(pop(lex));
		}
		
		if(!peek(lex) || peek(lex)->token_type == END || 
		   peek(lex)->token_type == THEN || peek(lex)->token_type == ELSE || 
		   peek(lex)->token_type == ELIF || peek(lex)->token_type == FI)
		{
			break;
		}
		
		struct ast_list *new_node = (struct ast_list *)init_ast_list();
		new_node->elt = parser_and_or(lex);
		current->next = new_node;
		current = new_node;
	}
	
	return (struct ast *)ast_list;
}

static struct ast *parser_elif(struct lex *lex)
{
	if(!peek(lex) || peek(lex)->token_type != ELIF)
		return NULL;
	discard_token(pop(lex));
	
	struct ast_if *ast_if = (struct ast_if *)init_ast_if();
	
	ast_if->condition = parser_compound_list(lex);
	
	if(!peek(lex) || peek(lex)->token_type != THEN)
	{
		free_ast((struct ast *)ast_if);
		return NULL;
	}
	discard_token(pop(lex));
	
	ast_if->then_body = parser_compound_list(lex);
	
	if(peek(lex) && (peek(lex)->token_type == ELSE || peek(lex)->token_type == ELIF))
	{
		ast_if->else_body = parser_else_clause(lex);
	}
	
	return (struct ast *) ast_if;
}

static struct ast *parser_else_clause(struct lex *lex)
{
	if(!peek(lex) || peek(lex)->token_type == END)
		return NULL;
		
	if(peek(lex)->token_type == ELIF)
	{
		return parser_elif(lex);
	}
	
	if(peek(lex)->token_type == ELSE)
	{
		discard_token(pop(lex));
		return parser_compound_list(lex);
	}
	
	return NULL;
}

static struct ast *parser_rule_if(struct lex *lex)
{
	if(!peek(lex) || peek(lex)->token_type != IF)
		return NULL;
	discard_token(pop(lex));
	
	struct ast_if *ast_if = (struct ast_if *)init_ast_if();
	
	ast_if->condition = parser_compound_list(lex);
	
	if(!peek(lex) || peek(lex)->token_type != THEN)
	{
		free_ast((struct ast *)ast_if);
		return NULL;
	}
	discard_token(pop(lex));
	ast_if->then_body = parser_compound_list(lex);
	
	if(peek(lex) && (peek(lex)->token_type == ELSE || peek(lex)->token_type == ELIF))
	{
		ast_if->else_body = parser_else_clause(lex);
	}
	
	if(!peek(lex) || peek(lex)->token_type != FI)
	{
		free_ast((struct ast *)ast_if);
		return NULL;
	}
	discard_token(pop(lex));
	
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

	if(peek(lex) && peek(lex)->token_type == WORD)
	{
		struct token *tok = pop(lex);
		ast_cmd->words[ind] = tok->value;
		free(tok);
		ind++;
		ast_cmd->words = realloc(ast_cmd->words, (ind+1) * sizeof(char *));
		ast_cmd->words[ind] = NULL;

		lex->context = WORD;
		while(peek(lex) && peek(lex)->token_type == WORD)
		{
			tok = pop(lex);
			ast_cmd->words[ind] = tok->value;
			free(tok);
			ind++;
			ast_cmd->words = realloc(ast_cmd->words, (ind+1) * sizeof(char *));
			ast_cmd->words[ind] = NULL;
		}
		lex->context = KEYWORD;
		return (struct ast *)ast_cmd;
	}
	lex->context = KEYWORD;
	free(ast_cmd->words);
	free(ast_cmd);
	return NULL;
}

static struct ast *parser_command(struct lex *lex)
{
	if(peek(lex) && peek(lex)->token_type == IF)
	{
		return parser_shell_command(lex);
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

static struct ast *parser_list(struct lex *lex)
{
	if(!peek(lex) || peek(lex)->token_type == END || peek(lex)->token_type == NEWLINE)
		return NULL;
	
	lex->context = KEYWORD;
	struct ast_list *ast_list = (struct ast_list *)init_ast_list();
	ast_list->elt = parser_and_or(lex);
	if(!ast_list->elt)
	{
		free(ast_list);
		return NULL;
	}
	if (peek(lex) && (peek(lex)->token_type == SEMI_COLON || peek(lex)->token_type == NEWLINE))
	{
		discard_token(pop(lex));
		if(peek(lex) && peek(lex)->token_type != END && peek(lex)->token_type != NEWLINE)
		{
			ast_list->next = (struct ast_list *)parser_list(lex);
		}
	}
	return (struct ast *)ast_list;
}

struct ast *parser(FILE *entry)
{
	struct lex *lex = init_lex(entry);
	lex->context = KEYWORD;
	if (!peek(lex))
	{
		free_lex(lex);
		return NULL;
	}
	
	while(peek(lex) && peek(lex)->token_type == NEWLINE)
	{
		discard_token(pop(lex));
	}
	
	if (!peek(lex) || peek(lex)->token_type == END)
	{
		free_lex(lex);
		return init_ast_list();
	}

	struct ast *ast = parser_list(lex);
	
	if (!ast)
	{
		free_lex(lex);
		return NULL;
	}
	
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
