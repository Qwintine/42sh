#include <stdio.h>
#include "parser.h"
#include "../lexer/lexer.h"

/*
 * Descriptif a faire
 */
struct token *peek(struct lex *lex)
{
	if(lex)
	{
		if(!lex->current_token)
		{
			lexer(lex);
		}
		return lex->current_token;
	}
	return NULL;
}


/*
 * Descriptif a faire
 */
struct token *pop(struct lex *lex)
{
	struct token *tok = lex->current_token;
	lex->current_token = NULL;
	return tok;
}

struct ast parser()
{
}

/*
 * * Probablement modif plus tard
 */
int parse_input(struct ast *input_ast)
{
	struct lex *lex = malloc(sizeof(struct lex ));
	lex->entry = calloc(1,sizeof(FILE *));
	lex->current_token = NULL;
	lex->context = NULL;

	struct ast *list_ast = malloc(sizeof(struct ast ));

	if(parse_list(ast, &lex))
	{
		if(peek(&lex) || ((peek(lex)->current_token!= END 
					&& peek(lex)->current_token != NEWLINE)))
		{
			//erreur
		}
		input_ast->tok = pop(&lex);
		input_ast->size = 0;
		input_ast->next = NULL;
		free(list_ast);
		return 0; // penser a gerer lex 
	}

	struct token *token = malloc(sizeof(struct token ));
	token->value = NULL;
	token->token_type = INPUT; // rajouter dans enum
	input_ast->tok = token;
	input_ast->next = malloc(sizeof(struct ast));
	input_ast->next = list_ast;
	input_ast->size = list_ast->size +1;
	return 0;
}


