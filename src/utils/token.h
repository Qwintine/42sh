#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>

enum type
{
	IF,
	THEN,
	ELIF,
	ELSE,
	FI,
	NEGATION,
	AND,
	OR,
	REDIR_OUT, // >
	REDIR_NO_CLOBB, // >|
	REDIR_IN, // <
	REDIR_APPEND, // >>
	REDIR_DUP_OUT, // >&
	REDIR_DUP_IN, // <&
	REDIR_IO, // <>
	PIPE,
	SEMI_COLON,
	NEWLINE,
	WORD,
	KEYWORD,
	END
};

struct token
{
    char *value;
    enum type token_type;
};

struct lex
{
    FILE *entry;
    struct token *current_token;
    enum type context;
};

struct lex *init_lex(FILE *entry);
struct token *init_token(enum type context);
void free_lex(struct lex *lex);
void free_token(struct token *tok);

#endif /* TOKEN_H */
