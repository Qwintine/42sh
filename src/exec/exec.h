#ifndef EXEC_H
#define EXEC_H

#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../built-in/echo.h"

int exec_cmd(char **words);

#endif /* !EXEC_H */
