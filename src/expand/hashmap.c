#define _POSIX_C_SOURCE 200809L
#include "expand.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../utils/itoa.h"
#include <pwd.h>

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

static void add_special(struct dictionnary *dict, char *key, char *val)
{
    char *varas = malloc(strlen(key) + strlen(val) + 2);
    varas = strcpy(varas,key);
    varas = strcat(varas, "=");
    varas = strcat(varas, val);
    add_var(dict,varas);
    free(varas);
    free(val);
}

/*Description:
 *	Initialise the dictionnary
 *Argument:
 *	The hashing function used by the dictionnary to hash the key
 */
struct dictionnary *init_dict(void)
{
    struct dictionnary *dict = malloc(sizeof(struct dictionnary));

    for (size_t i = 0; i < 20; i++)
    {
        dict->variables[i] = NULL;
    }

    for (size_t i = 0; i < 20; i++)
    {
        dict->function[i] = NULL;
    }

    add_special(dict, "$", itoa(getpid()));
    struct passwd *pw = getpwuid(getuid());
    add_special(dict, "UID", itoa((int)getuid()));
    if (pw && pw->pw_dir)
        add_special(dict, "HOME", strdup(pw->pw_dir));
    else
        add_special(dict, "HOME", strdup("/"));
    char* cwd = malloc(2048);
    cwd = getcwd(cwd, 2048);
    add_special(dict, "PWD", strdup(cwd));
    free(cwd);
    add_special(dict, "IFS", strdup(" \t\n"));
    //key[2] = "PATH";

    return dict;
}

char *special(char *key)
{
    if (!strcmp(key, "RANDOM"))
    {
        sleep(1);
        srand((unsigned)time(NULL));
        return itoa(rand());
    }
    if (!strcmp(key, "UID"))
        return itoa((int)getuid());
    return 0;
}

static int update_or_append_var(struct variables *bucket, struct variables *new,
                                char *key, char *val)
{
    struct variables *target = bucket;
    while (target)
    {
        if (strcmp(target->key, key) == 0)
        {
            free(target->elt[0]);
            target->elt[0] = val;
            free(key);
            free(new->elt);
            free(new); 
            return 0;
        }
        if (!target->next)
            break;
        target = target->next;
    }
    target->next = new;
    return 0;
}

/*Description:
 *  Add a variable to the dictionnary
 *Arguments:
 *  key: the variable name
 *  val: the variable value
 */
int add_var(struct dictionnary *dict, char *varas)
{
    size_t i = 0;
    while (varas && varas[i] && varas[i] != '=')
    {
        i++;
    }

    if (!dict || !varas || varas[i] != '=')
        return 1;

    char *key = malloc(i + 1);
    char *val = malloc(strlen(varas + i + 1) + 1);
    if (!key || !val)
    {
        goto ERROR;
    }

    strncpy(key, varas, i);
    key[i] = '\0';
    strcpy(val, varas + i + 1);
    char **to_ex = malloc(2 * sizeof(char *));
    to_ex[0] = val;
    to_ex[1] = NULL;
    char **expanded = expand(dict, to_ex);
    if(expanded && expanded[0])
    {
        free(val);
        val = expanded[0];
    }
    free(to_ex);
    free(expanded);

    struct variables *new = malloc(sizeof(struct variables));

    if (!new)
    {
        goto ERROR;
    }

    new->key = key;
    new->elt = malloc(2 * sizeof(char *));
    if (!new->elt)
    {
        free(new);
        goto ERROR;
    }
    new->elt[0] = val;
    new->elt[1] = NULL;
    new->next = NULL;

    int ind = hash(key);
    if (!dict->variables[ind])
    {
        dict->variables[ind] = new;
        return 0;
    }

    return update_or_append_var(dict->variables[ind], new, key, val);

ERROR:
    free(key);
    free(val);
    return 1;
}

int add_var_arg(struct dictionnary *dict, char *key, char **val)
{
    struct variables *new = malloc(sizeof(struct variables));

    if (!new)
    {
        goto ERROR;
    }

    new->key = strdup(key);
    size_t i = 0;
    while (val[i])
        i++;
    new->elt = malloc((i + 1) * sizeof(char *));
    if (!new->elt)
    {
        free(new);
        goto ERROR;
    }
    for (size_t j = 0; j < i; j++)
    {
        new->elt[j] = strdup(val[j]);
    }
    new->elt[i] = NULL;
    new->next = NULL;
    if (!new->elt)
    {
        free(new);
        goto ERROR;
    }

    int ind = hash(key);
    if (!dict->variables[ind])
    {
        dict->variables[ind] = new;
        return 0;
    }

    struct variables *target = dict->variables[ind];
    while (target->next)
    {
        if (strcmp(target->key, key) == 0)
        {
            free_val(new);
            target->elt = val;
            return 0;
        }
        target = target->next;
    }
    target->next = new;
    return 0;

ERROR:
    free(key);
    free(val);
    return 1;
}

int add_func(struct dictionnary *dict, char *key, ast *cmd_block)
{
    struct function *new = malloc(sizeof(struct function));
    new->key = key;
    new->ast = cmd_block;
    new->next = NULL;

    int ind = hash(key);

    if(!dict->function[ind])
    {
        dict->function[ind] = new;
        return 0;
    }

    struct function *target = dict->function[ind];
    while (target->next)
    {
        if (strcmp(target->key, key) == 0)
        {
            free_val(new);
            target->elt = val;
            return 0;
        }
        target = target->next;
    }
    target->next = new;
    return 0;
}

/*Description:
 *  Get the variable from the dictionnary
 *Arguments:
 *  key: the variable name
 */
char **get_var(struct dictionnary *dict, char *key)
{
    char *g = special(key);
    if (g)
    {
        char **res = malloc(2 * sizeof(char *));
        res[0] = g;
        res[1] = NULL;
        return res;
    }

    int ind = hash(key);

    struct variables *target = dict->variables[ind];
    while (target && strcmp(target->key, key) != 0)
    {
        target = target->next;
    }
    if (!target)
    {
        char **res = malloc(sizeof(char *));
        res[0] = NULL;
        if(!strcmp(key, "OLDPWD"))
        {
            free(res);
            return get_var(dict, "PWD");
        }        
        return res;
    }
    size_t i = 0;
    while (target->elt[i])
    {
        i++;
    }
    char **res = malloc((i+1) * sizeof(char *));
    size_t j = 0;
    while(j < i)
    {
        res[j] = strdup(target->elt[j]);
        j++;
    }
    res[j] = NULL;
    return res;
}

void free_var(struct variables *var)
{
    while (var)
    {
        struct variables *last = var;
        var = var->next;
        free(last->key);
        size_t i = 0;
        while (last->elt[i])
        {
            free(last->elt[i]);
            i++;
        }
        free(last->elt);
        free(last);
    }
}

void free_func(struct function *function)
{
    while (function)
    {
        struct function *last = function;
        function = function->next;
        free(last->key);
        free_ast(last->ast);
        free(last);
    }
}

void free_dict(struct dictionnary *dict)
{
    for (size_t i = 0; i < 20; i++)
    {
        free_var(dict->variables[i]);
        free_func(dict->function[i])
    }
    free(dict);
}
