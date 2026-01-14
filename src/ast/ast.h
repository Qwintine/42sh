#ifndef AST_H
#define AST_H

#include "../utils/token.h"

enum ast_type
{
    AST_LOOP,
    AST_CMD,
    AST_IF,
    AST_LIST
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

struct ast_cmd
{
    struct ast base;
    char **words;
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

typedef void (*ast_handler_free)(struct ast *);
typedef int (*ast_handler_run)(struct ast *);

struct ast *init_ast_list(void);
struct ast *init_ast_cmd(void);
struct ast *init_ast_if(void);
void free_ast(struct ast *node);
int run_ast(struct ast *node);

#endif /* AST_H */
