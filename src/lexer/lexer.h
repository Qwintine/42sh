#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "../utils/token.h"

// Permet de renvoyer les tokens un par un au parser depuis un stream de données
struct token *lexer(FILE *entry);

#endif /* LEXER_H */

