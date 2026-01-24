#include "itoa.h"

char *itoa(int num)
{
    int i = 0;
    char *res = malloc(20);
    if (!res)
        return NULL;
    if (num == 0)
    {
        res[0] = '0';
        res[1] = 0;
        return res;
    }

    while (num > 0)
    {
        res[i] = (num % 10) + 48;
        num = num / 10;
        i++;
    }

    res[i] = 0;

    for (int j = 0; j < i / 2; j++)
    {
        char c = res[j];
        res[j] = res[i - j - 1];
        res[i - j - 1] = c;
    }

    return res;
}