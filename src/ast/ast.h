#ifndef AST_H
#define AST_H

#include <stddef.h>
#include "../utils/token.h"

enum ast_type
{
	AST_CMD,
	AST_IF,
	AST_LIST
};

struct ast
{
	enum ast_type type;
};

struct ast_cmd 
{
	struct ast base;
	char **words;
};

struct ast_if
{
	struct ast base;
	struct ast *condition;
	struct ast *then_body;
	struct ast *else_body; // Nullable
};

struct ast_list
{
	struct ast base;
	struct ast *elt;
	struct ast_list *next;
}; 

#endif /* AST_H */
