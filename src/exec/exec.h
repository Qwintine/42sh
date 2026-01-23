#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../builtin/echo.h"
#include "../builtin/exit.h"
#include "../builtin/cd.h"
#include "../builtin/export.h"
#include "../expand/expand.h"
#include "../utils/itoa.h"
#include "redir_exec.h"
#include "../ast/ast.h"
#include "../utils/redir.h"

int exec_cmd(struct ast_cmd *ast_cmd, struct dictionnary *vars, int *exit);

int exec_pipe(struct ast_cmd **cmd, int fd[2], struct dictionnary *vars, int *exit);

#endif /* !EXEC_H */
