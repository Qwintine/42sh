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
	WORD,
	COMMAND,
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

#endif /* TOKEN_H */
