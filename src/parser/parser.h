#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include <stdio.h>

struct ast *parser(FILE *entry);

#endif /* PARSER_H */
