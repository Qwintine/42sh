#include <stdlib.h>

#include "../utils/redir.h"
#include "parser_aux.h"

int parser_redir(struct lex *lex, struct ast_cmd *ast_cmd)
{
    struct redir *redir = init_redir();
    if (redir)
    {
        if (peek(lex) && peek(lex)->token_type == IO_NUMBER)
        {
            redir->io_num = peek(lex)->value;
            free(pop(lex));
        }

        if (!peek(lex) || !is_redir(peek(lex)->token_type))
        {
            free_redir(redir);
            return 1;
        }
        redir->type = peek(lex)->token_type;
        discard_token(pop(lex));
        lex->context = WORD;
        if (!peek(lex) || peek(lex)->token_type != WORD)
        {
            free_redir(redir);
            return 1;
        }
        redir->target = peek(lex)->value;
        free(pop(lex));
        size_t ind = 0;
        while (ast_cmd->redirs[ind])
        {
            ind++;
        }
        ast_cmd->redirs[ind++] = redir;
        ast_cmd->redirs =
            realloc(ast_cmd->redirs, (ind + 1) * sizeof(struct redir *));

        if (!ast_cmd->redirs)
        {
            return 1;
        }
        ast_cmd->redirs[ind] = NULL;
        return 0;
    }
    return 1;
}

int parser_element(struct lex *lex, struct ast_cmd *ast_cmd, size_t *w)
{
    if (peek(lex))
    {
        if (peek(lex)->token_type == WORD)
        {
            struct token *tok = pop(lex);
            if (!tok)
                return 1;
            ast_cmd->words[*w] = tok->value;
            free(tok);
            (*w)++;
            ast_cmd->words = realloc(ast_cmd->words, (*w + 1) * sizeof(char *));
            if (!ast_cmd->words)
                return 1;
            ast_cmd->words[*w] = NULL;
        }
        else if (peek(lex)->token_type == IO_NUMBER
                 || is_redir(peek(lex)->token_type))
        {
            return parser_redir(lex, ast_cmd);
        }
        return 0;
    }
    return 1;
}

/* TODO */
int parser_prefix(struct lex *lex, struct ast_cmd *ast_cmd)
{
    if (peek(lex) && peek(lex)->token_type == ASSIGNMENT)
    {
        size_t i = 0;
        while (ast_cmd->assignment[i])
            i++;

		struct token *tok = pop(lex);
        ast_cmd->assignment[i] = tok->value;
        i++;
        free(tok);
        ast_cmd->assignment = realloc(ast_cmd->assignment, i + 1);
        ast_cmd->assignment[i] = NULL;
        return 0;
    }
    return parser_redir(lex, ast_cmd);
}
