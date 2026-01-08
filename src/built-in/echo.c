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
    size_t size = strlen(str) + 1;
    char c = str[ind];
    char nc = 0;
    if (c == '\n')
        nc = 'n';
    if (c == '\t')
        nc = 't';
    char *res = malloc(size + 1);
    res = strncpy(res, str, ind);
    res[ind] = '\\';
    res[ind + 1] = nc;
    ind += 2;
    while (ind < size + 1)
    {
        res[ind] = str[ind - 1];
        ind++;
    }
    free(str);
    return res;
}

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
    for (; i < size; i++)
    {
        if (!e)
        {
            size_t ind = 0;
            while (strings[i][ind] != 0)
            {
                char c = strings[i][ind];
                if (c == '\n' || c == '\t')
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
