#define _POSIX_C_SOURCE 200809L
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer_aux.h"

/*
 * Description:
 *	Helper; Concat char to token value
 * Arguments:
 *	char *val -> current token value
 *	char c -> char to concat
 * Return:
 *	New token value
 */
char *concat(char *val, char c)
{
    size_t size = strlen(val);
    char *new_val = realloc(val, size + 2);
    if (!new_val)
        return NULL;
    new_val[size] = c;
    new_val[size + 1] = '\0';
    return new_val;
}

int all_digit(char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (str[i] < '0' || str[i] > '9')
            return 0;
    }
    return 1;
}

int valid_io(char *str)
{
    if (!all_digit(str))
        return 0;

    size_t len = strlen(str);
    if (len > 10)
        return 0;

    char *endptr;
    long num = strtol(str, &endptr, 10);

    if (*endptr != '\0' || num < 0) // Check for errors
        return 0;

    // Get max nb fd
    long max_fd = sysconf(_SC_OPEN_MAX);
    if (max_fd == -1)
    {
        max_fd = 1024;
    }

    if (max_fd > INT_MAX)
        max_fd = INT_MAX;

    if (num > max_fd)
        return 0;

    return 1;
}
