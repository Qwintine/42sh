#define _POSIX_C_SOURCE 200809L
#include <stdio.h>

#include "lexer_aux.h"

/*
 * Description:
 * 	Handle a possible operator
 * Argument:
 * 	lex -> struct lexer
 * 	tok -> token being created
 * 	buf[] -> current character
 * Return:
 * 	0 Token created / 1 Error
 */
int manage_op(struct lex *lex, struct token *tok, char buf[])
{
    char first = tok->value[0];
    if (first == '&')
    {
        if (buf[0] == '&')
        {
            tok->value = concat(tok->value, '&');
            if (!tok->value)
            {
                return 1;
            }
            tok->token_type = AND;
        }
        else
        {
            tok->token_type = SEMI_COLON;
            fseek(lex->entry, -1, SEEK_CUR);
        }
    }
    else
    {
        if (buf[0] == '|')
        {
            tok->value = concat(tok->value, '|');
            if (!tok->value)
            {
                return 1;
            }
            tok->token_type = OR;
        }
        else
        {
            tok->token_type = PIPE;
            fseek(lex->entry, -1, SEEK_CUR);
        }
    }
    lex->current_token = tok;
    return 0;
}

/*
 * Description:
 * 	Handle different redirections
 * Arguments:
 * 	lex-> struct lexer
 * 	tok -> token being created
 *      buf[] -> currrent character
 * Return:
 * 	0 Redir token created / 1 Error
 */
int manage_redir(struct lex *lex, struct token *tok, char buf[])
{
    if (!tok->value || !tok->value[0]) // garde fou ( #wankilcrazy )
        return 1;

    char first = tok->value[0];
    if (first == '>') // starts with >
    {
        if (buf[0] == '>')
        {
            tok->value = concat(tok->value, '>');
            tok->token_type = REDIR_APPEND; // create token >>
        }
        else if (buf[0] == '&')
        {
            tok->value = concat(tok->value, '&');
            tok->token_type = REDIR_DUP_OUT; // create token >&
        }
        else if (buf[0] == '|')
        {
            tok->value = concat(tok->value, '|');
            tok->token_type = REDIR_NO_CLOBB; // create token >|
        }
        else
        {
            tok->token_type = REDIR_OUT; // token >
            fseek(lex->entry, -1, SEEK_CUR);
        }
    }
    else // <
    {
        if (buf[0] == '>')
        {
            tok->value = concat(tok->value, '>');
            tok->token_type = REDIR_IO; // create token <>
        }
        else if (buf[0] == '&')
        {
            tok->value = concat(tok->value, '&');
            tok->token_type = REDIR_DUP_IN; // create token <&
        }
        else
        {
            tok->token_type = REDIR_IN;
            fseek(lex->entry, -1, SEEK_CUR);
        }
    }
    if (!tok->value)
        return 1;
    lex->current_token = tok;
    return 0;
}
