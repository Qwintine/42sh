#ifndef AST_H
#define AST_H

#include <stddef.h>
#include "../utils/token.h"

struct ast
{
	struct token *tok;
	size_t size;
	struct ast *next;
};


#endif /* AST_H */
