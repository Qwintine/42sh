#ifndef AST_H
#define AST_H

#include "../expand/expand.h"
#include "../utils/redir.h"
#include "../utils/token.h"

enum ast_type
{
    AST_LOOP,
    AST_PIPE,
    AST_CMD,
    AST_IF,
    AST_LIST,
    AST_AND_OR,
    AST_SHELL_REDIR,
    AST_FOR,
};

struct ast
{
    enum ast_type type;
};

struct ast_loop
{
    struct ast base;
    int truth;
    struct ast *condition;
    struct ast *body;
};

struct ast_for
{
    struct ast base;
    char *var;
    char **words;
    struct ast *body;
};

struct ast_pipe
{
    struct ast base;
    int negation;
    struct ast_cmd **cmd;
};

struct ast_cmd
{
    struct ast base;
    char **assignment;
    char **words; // arguments de la commande
    struct redir **redirs; // redir à appliquer dans l'ordre
};

struct ast_if
{
    struct ast base;
    struct ast *condition;
    struct ast *then_body;
    struct ast *else_body; // Nullable
};

struct ast_list
{
    struct ast base;
    struct ast *elt;
    struct ast_list *next;
};

struct ast_and_or
{
    struct ast base;
    struct ast *left;
    struct ast *right;
    enum type operator;
};

struct ast_shell_redir
{
    struct ast base;
    char **words;
    struct redir **redirs;
    struct ast *child; // shell_command (if/while/until)
};

typedef void (*ast_handler_free)(struct ast *);
typedef int (*ast_handler_run)(struct ast *, struct dictionnary *, int *);

void free_ast(struct ast *node);
int run_ast(struct ast *node, struct dictionnary *vars, int *exit);

#endif /* AST_H */
