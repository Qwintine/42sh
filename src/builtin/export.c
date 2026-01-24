#define  _POSIX_C_SOURCE 200112L
#include "export.h"
#include <ctype.h>

static int is_valid_name(const char *name)
{
    if (!name || !*name || isdigit(*name))
        return 0;
    for (size_t i = 0; name[i] && name[i] != '='; i++)
    {
        if (!isalnum(name[i]) && name[i] != '_')
            return 0;
    }
    return 1;
}

int export_b(char **args, struct dictionnary *dict)
{
    int ret = 0;
    for (size_t i = 0; args[i] != NULL; i++)
    {
        char *arg = args[i];
        
        if (!is_valid_name(arg))
        {
            ret = 1;
            continue;
        }
        
        int equal = 0;
        size_t j = 0; 
        while(arg[j]!='=' && arg[j]!=0)
        { 
            j++;
        }
        equal = arg[j]=='=';
        char *key = "";
        char *val = "";
        if(equal)
        {
            add_var(dict,arg);
            arg[j] = 0;
            key = arg;
            val = arg+j+1;
            setenv(key,val,1);
        }
        else
        {
            key = arg;
            char **var = get_var(dict, key);
            if (var && var[0])
            {
                val = var[0];
            }
            else
            {
                val = "";
            }
            setenv(key, val, 1);
            if (var)
                free(var);
        }
    }
    return ret;
}