#include "parser_aux.h"

/*
 * Description:
 * 	Peek on the lexer to see the current token
 * Arguments:
 * 	lex -> struct lexer being used
 * Return:
 * 	Current_token if success, NULL otherwise
 */
struct token *peek(struct lex *lex)
{
    if (lex)
    {
        if (lex->error)
            return NULL;
        if (!lex->current_token)
        {
            int res = lexer(lex);
            if (res)
            {
                lex->error = 1;
                return NULL;
            }
        }
        return lex->current_token;
    }
    return NULL;
}

/*
 * Description:
 * 	Pop the current token from lex
 * Arguments:
 * 	lex -> struct lexer being used
 * Return:
 * 	Copy of current_token if success, NULL otherwise
 */
struct token *pop(struct lex *lex)
{
    if (lex)
    {
        if (!lex->current_token)
        {
            if (!peek(lex))
                return NULL;
        }
        struct token *tok = lex->current_token;
        lex->current_token = NULL;
        return tok;
    }
    return NULL;
}

/*
 * Description:
 * 		Free a discarded token
 * Arguments:
 * 		Token to free
 * Return:
 * 		1 Succes / 0 Error
 */
int discard_token(struct token *tok)
{
    if (!tok)
    {
        return 0;
    }
    free_token(tok);
    return 1;
}

/* Helper pour savoir si token de type Redir */
int is_redir(enum type type)
{
    return type == REDIR_OUT || type == REDIR_IN || type == REDIR_APPEND
        || type == REDIR_DUP_OUT || type == REDIR_DUP_IN
        || type == REDIR_NO_CLOBB || type == REDIR_IO;
}
