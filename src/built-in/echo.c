#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Description:
 *  	print size strings sur le terminal
 * Arguments:
 *  	size: le nombre d'elements a print
 *  	strings: les elements a print
 * Retour:
 *  	0 si succes, 1 sinon
 * Verbose:
 *  	parse les option -neE si presente  en premiere position
 */
static char *insert_in(size_t ind, char *str)
{
    size_t size = strlen(str) +1;
    char c = str[ind];
    char nc = 0;
    switch (c)
    {
    case '\a':
        nc = 'a';
        break;
    case '\b':
        nc = 'b';
        break;
    case '\f':
        nc = 'f';
        break;
    case '\n':
        nc = 'n';
        break;
    case '\r':
        nc = 'r';
        break;
    case '\t':
        nc = 't';
        break;
    case '\v':
        nc = 'v';
        break;
    default:
        break;
    }
    char *res = malloc(size + 1);
    res = strncpy(res, str, ind);
    res[ind] = '\\';
    res[ind + 1] = nc;
    ind += 2;
    while (ind < size+1)
    {
        res[ind] = str[ind - 1];
		ind++;
    }
    free(str);
    return res;
}

int echo_b(size_t size, char **strings)
{
    int n = 0;
    int e = 0;
    size_t i = 0;
    if ((*strings)[0] == '-')
    {
        i++;
        size_t ind = 1;
        while ((*strings)[ind] != 0)
        {
            switch ((*strings)[ind])
            {
            case 'n':
                n = 1;
                break;
            case 'e':
                e = 1;
                break;
            case 'E':
                e = 0;
            default:
                break;
            }
            ind++;
        }
    }
    // TODO: \c et \e si e (\0 et \x itou?)
    for (; i < size; i++)
    {
        if (!e)
        {
            size_t ind = 0;
            while (strings[i][ind] != 0)
            {
                char c = strings[i][ind];
                if (c == '\a' || c == '\b' || c == '\f' || c == '\n'
                    || c == '\r' || c == '\t' || c == '\v')
                {
                    strings[i] = insert_in(ind, strings[i]);
                }
                ind++;
            }
        }
        printf("%s", strings[i]);
        if (i < size - 1)
            printf(" ");
    }
    if (!n)
    {
        printf("\n");
    }
    return 0;
}
