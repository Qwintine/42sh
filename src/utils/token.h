#ifndef TOKEN_H
#define TOKEN_H

enum type
{
	IF,
	THEN,
	ELIF,
	ELSE,
	FI,
	SEMI_COLON,
	NEWLINE,
	QUOTE,
	WORD
};

struct token
{
	char *value;
	enum type token_type;
};

#endif /* TOKEN_H */
