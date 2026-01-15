#ifndef EXEC_H
#define EXEC_H
#include "../ast/ast.h"
#include "../utils/redir.h"

int exec_cmd(char **words, struct redir **redirs);

int exec_pipe(struct ast_cmd **cmd, int fd[2]);

#endif /* !EXEC_H */
