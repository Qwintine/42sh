
#include "exit.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Description:
 * 	Check if str valid number
 * Arguments:
 * 	char *str -> string to check
 * Return:
 * 	int -> 1 valid number, 0 otherwise
 */
int is_valid_nb(char *str)
{
    if (*str == '-' || *str == '+')
        str++;
    if (*str == '\0')
        return 0;
    while (*str)
    {
        if (*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}

/* Description:
 * 	Execute exit builtin cmd
 * Arguments:
 * 	char **codes -> args passed to exit
 * 	int *exit -> pointer to exit status
 * Return:
 * 	int -> Exit status of exit cmd
 */
int exit_b(char **codes, int *exit)
{
    if (codes[0] != NULL && codes[1] != NULL) // too much args
    {
        fprintf(stderr, "42sh: exit: too many args\n");
        *exit = 1;
        return 1;
    }

    if (codes[0] != NULL && !is_valid_nb(codes[0])) // arg not number
    {
        fprintf(stderr, "42sh: exit: arg not num\n");
        *exit = 1;
        return 2;
    }

    int exit_code = 0;
    if (codes[0] != NULL)
    {
        exit_code = atoi(codes[0]);
    }
    if (exit_code < 0 || exit_code > 255)
    {
        if (exit_code < 0)
            exit_code = 256 + (exit_code % 256);
        else
            exit_code = exit_code % 256;
    }
    *exit = 1;
    return exit_code;
}
