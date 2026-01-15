#ifndef EXEC_H
#define EXEC_H
#include "../utils/redir.h"
#include "../ast/ast.h"

int exec_cmd(char **words, struct redir **redirs);

int exec_pipe(struct ast_cmd **cmd, int fd[2]);

#endif /* !EXEC_H */
