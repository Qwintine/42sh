#ifndef AST_AUX_H
#define AST_AUX_H

#include "ast.h"

// ast_init.c
struct ast *init_ast_list(void);
struct ast *init_ast_cmd(void);
struct ast *init_ast_if(void);
struct ast *init_ast_pipe(void);
struct ast *init_ast_loop(void);
struct ast *init_ast_and_or(void);
struct ast *init_ast_shell_redir(void);
struct ast *init_ast_for(void);

// ast_free.c
void ast_free_list(struct ast *ast);
void ast_free_cmd(struct ast *ast);
void ast_free_if(struct ast *ast);
void ast_free_pipe(struct ast *ast);
void ast_free_loop(struct ast *ast);
void ast_free_for(struct ast *ast);
void ast_free_and_or(struct ast *ast);
void ast_free_shell_redir(struct ast *ast);

// ast_run.c
int ast_run_list(struct ast *ast, struct dictionnary *vars, int *exit);
int ast_run_cmd(struct ast *ast, struct dictionnary *vars, int *exit);
int ast_run_if(struct ast *ast, struct dictionnary *vars, int *exit);
int ast_run_pipe(struct ast *ast, struct dictionnary *vars, int *exit);
int ast_run_loop(struct ast *ast, struct dictionnary *vars, int *exit);
int ast_run_for(struct ast *ast, struct dictionnary *vars, int *exit);
int ast_run_and_or(struct ast *ast, struct dictionnary *vars, int *exit);
int ast_run_shell_redir(struct ast *ast, struct dictionnary *vars, int *exit);

#endif /* AST_AUX_H */
