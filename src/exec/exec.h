#ifndef EXEC_H
#define EXEC_H

#include "../ast/ast.h"
#include "../expand/expand.h"

int exec_cmd(struct ast_cmd *ast_cmd, struct dictionnary *vars, int *exit);

int exec_pipe(struct ast_cmd **cmd, int fd[2], struct dictionnary *vars, int *exit);

#endif /* !EXEC_H */
