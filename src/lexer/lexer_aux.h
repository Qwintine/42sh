#ifndef LEXER_AUX_H
#define LEXER_AUX_H

#include <stdio.h>

#include "../utils/token.h"
#include "lexer.h"

// lexer_utils.c
char *concat(char *val, char c);
int all_digit(char *str);
int valid_io(char *str);

// lexer_handlers.c
int handle_com(int in_quotes, struct lex *lex, struct token *tok, char *buf);
int handle_backslash(char **value, FILE *entry, int in_quotes);
int handle_quote(int *quote, int other_quote, struct token *tok);
int handle_newline(struct token *tok, int quote, FILE *entry);
int handle_semicolon(struct token *tok, int quote, FILE *entry);
int handle_blank(struct token *tok, char c, int quote);
int handle_delimiter(struct token *tok, char c, int quote, FILE *entry);
int new_op(struct token *tok, int quote, FILE *entry, char val);

// lexer_tokens.c
struct token *end_token(struct token *tok, struct lex *lex);
int verif_token_if(struct token *tok, enum type context);
int verif_token_loop(struct token *tok, enum type context);
int verif_token(struct token *tok, enum type context);
enum type check_type(char *value);

// lexer_operators.c
int manage_op(struct lex *lex, struct token *tok, char buf[]);
int manage_redir(struct lex *lex, struct token *tok, char buf[]);

#endif /* LEXER_AUX_H */
