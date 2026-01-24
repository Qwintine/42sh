#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "../utils/token.h"

struct quote_status
{
    int double_quote;
    int single_quote;
    int bracket_open;
};

// Permet de renvoyer les tokens un par un au parser depuis un stream de données
int lexer(struct lex *lex);

#endif /* LEXER_H */
