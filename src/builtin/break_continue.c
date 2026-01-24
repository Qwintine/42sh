#include "break_continue.h"

#include <stdio.h>
#include <stdlib.h>

static int break_n = 0;
static int continue_n = 0;

int get_break(void)
{
    return break_n;
}

int get_continue(void)
{
    return continue_n;
}

void set_break(void)
{
    if (break_n > 0)
        break_n--;
}

void update_continue(void)
{
    if (continue_n > 0)
        continue_n--;
}

static int is_nb(char *str)
{
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

int break_b(char **args)
{
    if (args[0] != NULL && args[1] != NULL)
        return 1;

    int n = 1;
    int res = 0;

    if (args[0] != NULL)
    {
        if (!is_nb(args[0]))
        {
            n = 1;
            res = 128;
        }
        else
        {
            n = atoi(args[0]);
            if (n <= 0)
            {
                n = 1;
                res = 128;
            }
        }
    }

    break_n = n;
    return res;
}

int continue_b(char **args)
{
    if (args[0] != NULL && args[1] != NULL)
        return 1;

    int n = 1;
    int res = 0;

    if (args[0] != NULL)
    {
        if (!is_nb(args[0]))
        {
            n = 1;
            res = 128;
        }
        else
        {
            n = atoi(args[0]);
            if (n <= 0)
            {
                n = 1;
                res = 128;
            }
        }
    }

    continue_n = n;
    return res;
}
