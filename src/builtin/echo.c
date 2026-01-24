#include "echo.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Description:
 *  	insert 2 character
 * Arguments:
 *  	ind: l'indice du char a changer
 *  	str: string
 * Retour:
 *  	le string modifier
 * Verbose:
 *  	".....\n......." => ".....\\n......."
 */
static char *insert_in(size_t ind, char *str)
{
    size_t size = strlen(str);
    char c = str[ind + 1];
    char replacement = 0;

    if (c == 'n')
        replacement = '\n';
    else if (c == 't')
        replacement = '\t';
    else if (c == '\\')
        replacement = '\\';
    else
        return str;

    char *res = malloc(size + 1);
    if (!res)
        return str;
    res = strncpy(res, str, ind);
    res[ind] = replacement;
    ind++;
    while (ind < size)
    {
        res[ind] = str[ind + 1];
        ind++;
    }
    free(str);
    return res;
}

static int args_echo(char *string, int *an, int *ae)
{
    size_t i = 1;
    int n = *an;
    int e = *ae;
    while (string[i] != 0)
    {
        switch (string[i])
        {
        case 'n':
            n = 1;
            break;
        case 'e':
            e = 1;
            break;
        case 'E':
            e = 0;
            break;
        default:
            return 1;
        }
        i++;
    }
    *an = n;
    *ae = e;
    return 0;
}

/* Description:
 *  	print les strings sur le terminal
 * Arguments:
 *  	strings: les elements a print
 * Retour:
 *  	0 si succes, 1 sinon
 * Verbose:
 *  	parse les option -neE si presente en premiere position
 */
int echo_b(char **strings)
{
    int n = 0;
    int e = 0;
    int a = 1;
    size_t i = 0;
    for (; strings[i] != NULL; i++)
    {
        if (a)
        {
            if (strings[i][0] == '-')
            {
                if (args_echo(strings[i], &n, &e))
                    a = 0;
            }
            else
                a = 0;
        }
        if (!a)
        {
            if (e)
            {
                size_t ind = 0;
                while (strings[i][ind] != 0)
                {
                    char c = strings[i][ind];
                    if (c == '\\')
                    {
                        strings[i] = insert_in(ind, strings[i]);
                    }
                    ind++;
                }
            }
            printf("%s", strings[i]);
            if (strings[i][0] != 0 && strings[i + 1])
                printf(" ");
        }
    }
    if (!n)
    {
        printf("\n");
    }
    fflush(stdout);
    return 0;
}
