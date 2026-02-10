#include "hash.h"

#include <stddef.h>

/** hashing function for the dictionnary*/
int hash(char *str)
{
    size_t res = 0;
    for (size_t i = 0; str[i] != 0; i++)
    {
        res += str[i];
    }
    res *= res;
    res = res % 20;
    int r = res;
    return r;
}
