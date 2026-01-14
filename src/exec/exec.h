#ifndef EXEC_H
#define EXEC_H

#include "../ast/ast.h"

int exec_cmd(char **words);

int exec_pipe(struct ast_cmd **cmd, int fd[2]);

#endif /* !EXEC_H */
