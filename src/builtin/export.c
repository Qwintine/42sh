#define  _POSIX_C_SOURCE 200112L
#include "export.h"

int export_b(char **args, struct dictionnary *dict)
{
    for (size_t i = 0; args[i] != NULL; i++)
    {
        char *arg = args[i];
        int equal = 0;
        size_t i = 0; 
        while(arg[i]!='=' && arg[i]!=0)
        { 
            i++;
        }
        equal = arg[i]=='=';
        char *key = "";
        char *val = "";
        if(equal)
        {
            add_var(dict,arg);
            arg[i] = 0;
            key = arg;
            val = arg+i+1;
        }
        else
        {
            key = arg;
            char **var = get_var(dict, key);
            val = var[0];
            free(var);
        }
        setenv(key,val,1);
    }
    return 0;
}