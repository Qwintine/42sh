#ifndef IO_H
#define IO_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include "../expand/expand.h"
#include <string.h>

FILE *arg_file(int argc, char **argv, int *prettyprint, struct dictionnary *vars);
void free_stdin_buffer(void);
char *itoa(int num);

#endif /* IO_H */
