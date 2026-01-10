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
    char nc = 0;
    if (c == 'n')
        nc = '\n';
    else if (c == 't')
        nc = '\t';
    else if (c == '\\')
        nc = '\\';
    char *res = malloc(size);
    res = strncpy(res, str, ind);
    res[ind] = nc;
    ind++;
    while (ind < size)
    {
        res[ind] = str[ind + 1];
        ind++;
    }
    free(str);
    return res;
}

static int args_echo(int ind, int *an, int *ae)
{
    size_t i = 1;
    while (strings[ind][i] != 0)
    {
        switch (strings[ind][i])
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
            return 0;
        }
        i++;
    }
    *an = n;
    *ae = e;
    return 1;
}

/* Description:
 *  	print les strings sur le terminal
 * Arguments:
 *  	strings: les elements a print
 * Retour:
 *  	0 si succes, 1 sinon
 * Verbose:
 *  	parse les option -neE si presente  en premiere position
 */
int echo_b(char **strings)
{
    int n = 0;
    int e = 0;
    int a = 1;
    size_t i = 0;
    for (; strings[i] != NULL; i++)
    {
        if (a && strings[i][0] == '-')
            if (args(ind, &n, &e))
                a = 0;
        if (!a && e)
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
        if (strings[i + 1] != NULL)
            printf(" ");
    }
    if (!n)
    {
        printf("\n");
    }
    fflush(stdout);
    return 0;
}
