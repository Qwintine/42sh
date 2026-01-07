#include <stddef.h>
#include <stdio.h>

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
    if (*strings[0] == '-')
    {
        i++;
        size_t ind = 1;
        while (*strings[ind] != 0)
        {
            switch (*strings[ind])
            {
            case 'n':
                n = 1;
                break;
            case 'e':
                e = 1;
                break;
            case 'E':
                e = 0;
            }
            ind++;
        }
    }
    // TODO: \c et \e si e (\0 et \x itou?)
    for (; i < size; i++)
    {
        if (e)
        {
            size_t ind = 0;
            while (strings[i][ind] != 0)
            {
                char c = strings[i][ind];
                if (c == '\a' || c == '\b' || c == '\f' || c == '\n'
                    || c == '\r' || c == '\t' || c == '\v')
                {
                    // change to '\\'+'char';
                }
                ind++;
            }
        }
        printf("%s", strings[i]);
    }
    if (n)
    {
        printf("\n");
    }
    return 0;
}
