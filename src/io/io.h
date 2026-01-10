#ifndef IO_H
#define IO_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

FILE *arg_file(int argc, char **argv, int *prettyprint);

#endif /* IO_H */
