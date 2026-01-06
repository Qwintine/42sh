#ifndef LEXER_H
#define LEXER_H

// Permet de renvoyer les tokens un par un au parser depuis un stream de données
struct token lexer(FILE *entry);

#endif /* LEXER_H */

