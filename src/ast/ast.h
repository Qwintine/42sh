#ifndef AST_H
#define AST_H

#include "../utils/token.h"

struct ast
{
	struct token *tok;
	int size;
	struct ast *next;
};


#endif /* AST_H */
