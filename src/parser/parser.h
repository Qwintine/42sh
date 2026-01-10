#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

#include "../ast/ast.h"

struct ast *parser(FILE *entry);

#endif /* PARSER_H */
