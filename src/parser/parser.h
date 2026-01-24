#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

#include "../ast/ast.h"

struct ast *parser(FILE *entry, int *eof, struct dictionnary *dict);

#endif /* PARSER_H */
