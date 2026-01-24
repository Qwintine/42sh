#define _POSIX_C_SOURCE 200809L
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "../utils/itoa.h"
#include "expand.h"

// hashing function for the dictionnary
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
    if (!varas)
    {
        free(val);
        return;
    }
    varas = strcpy(varas, key);
    varas = strcat(varas, "=");
    varas = strcat(varas, val);
    add_var(dict, varas);
    free(varas);
    free(val);
}

/*Description:
 *	Initialise the dictionnary and adds special variables from the get go
 *Argument:
 *	The hashing function used by the dictionnary to hash the key
 */
struct dictionnary *init_dict(void)
{
    struct dictionnary *dict = malloc(sizeof(struct dictionnary));
    if (!dict)
        return NULL;

    for (size_t i = 0; i < 20; i++)
    {
        dict->variables[i] = NULL;
    }

    for (size_t i = 0; i < 20; i++)
    {
        dict->function[i] = NULL;
    }

    char *pid_str = itoa(getpid());
    if (pid_str)
        add_special(dict, "$", pid_str);

    struct passwd *pw = getpwuid(getuid());
    char *uid_str = itoa((int)getuid());
    if (uid_str)
        add_special(dict, "UID", uid_str);

    if (pw && pw->pw_dir)
    {
        char *home = strdup(pw->pw_dir);
        if (home)
            add_special(dict, "HOME", home);
    }
    else
    {
        char *home = strdup("/");
        if (home)
            add_special(dict, "HOME", home);
    }

    char *cwd = malloc(2048);
    if (cwd)
    {
        if (getcwd(cwd, 2048))
        {
            char *pwd = strdup(cwd);
            if (pwd)
                add_special(dict, "PWD", pwd);
        }
        free(cwd);
    }

    char *ifs = strdup(" \t\n");
    if (ifs)
        add_special(dict, "IFS", ifs);

    return dict;
}

// special variables handling
char *special(char *key)
{
    if (!strcmp(key, "RANDOM"))
    {
        sleep(1);
        srand((unsigned)time(NULL));
        return itoa(rand());
    }
    if (!strcmp(key, "PATH"))
    {
        char *path = getenv("PATH");
        if (path)
            return strdup(path);
        else
            return strdup("");
    }
    return 0;
}

// helper function to update or append a variable in the dictionnary
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
            if (getenv(target->key) != NULL)
                setenv(target->key, val, 1);
            return 0;
        }
        if (!target->next)
            break;
        target = target->next;
    }
    target->next = new;
    return 0;
}

static char *expand_value(struct dictionnary *dict, char *val)
{
    char **to_ex = malloc(2 * sizeof(char *));
    to_ex[0] = val;
    to_ex[1] = NULL;
    char **expanded = expand(dict, to_ex);
    if (expanded && expanded[0])
    {
        free(val);
        val = expanded[0];
    }
    free(to_ex);
    free(expanded);
    return val;
}

/*Description:
 *  Add a variable to the dictionnary
 *Arguments:
 *  varas: the variable as a string "KEY=VALUE"
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
        goto ERROR;

    strncpy(key, varas, i);
    key[i] = '\0';
    strcpy(val, varas + i + 1);
    val = expand_value(dict, val);

    struct variables *new = malloc(sizeof(struct variables));

    if (!new)
        goto ERROR;

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

    int ret = update_or_append_var(dict->variables[ind], new, key, val);
    return ret;

ERROR:
    free(key);
    free(val);
    return 1;
}

// helper function to duplicate values into the variable element
static int dup_val_to_elt(struct variables *new, char **val, size_t i)
{
    for (size_t j = 0; j < i; j++)
    {
        new->elt[j] = strdup(val[j]);
        if (!new->elt[j])
        {
            for (size_t k = 0; k < j; k++)
                free(new->elt[k]);
            free(new->elt);
            free(new->key);
            free(new);
            return 1;
        }
    }
    return 0;
}

/*Description:
 *  Add a variable with multiple arguments to the dictionnary
 *Arguments:
 *  key: the variable name
 *  val: the variable value as a list of strings
 *Extra:
 *  Especially useful for handling the $@ variable
 */
int add_var_arg(struct dictionnary *dict, char *key, char **val)
{
    struct variables *new = malloc(sizeof(struct variables));

    if (!new)
    {
        goto ERROR;
    }

    new->key = strdup(key);
    if (!new->key)
    {
        free(new);
        goto ERROR;
    }
    size_t i = 0;
    while (val[i])
        i++;
    new->elt = malloc((i + 1) * sizeof(char *));
    if (!new->elt)
    {
        free(new->key);
        free(new);
        goto ERROR;
    }
    if (dup_val_to_elt(new, val, i))
        goto ERROR;
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
            free_var(new);
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

/*Description:
 *  Add a function to the dictionnary
 *Arguments:
 *  key: the function name
 *  cmd_block: the function body as an AST
 */
int add_func(struct dictionnary *dict, char *key, struct ast *cmd_block)
{
    struct function *new = malloc(sizeof(struct function));
    new->key = key;
    new->ast = cmd_block;
    new->next = NULL;

    int ind = hash(key);

    if (!dict->function[ind])
    {
        dict->function[ind] = new;
        return 0;
    }

    struct function *target = dict->function[ind];
    while (target)
    {
        if (strcmp(target->key, key) == 0)
        {
            free_ast(target->ast);
            target->ast = cmd_block;
            free(new->key);
            free(new);
            return 0;
        }
        if (!target->next)
        {
            target->next = new;
            return 0;
        }
        if (!target->next)
            break;
        target = target->next;
    }
    return 1;
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
        if (!res)
        {
            free(g);
            return NULL;
        }
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
        if (!res)
            return NULL;
        res[0] = NULL;
        if (!strcmp(key, "OLDPWD"))
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
    char **res = malloc((i + 1) * sizeof(char *));
    if (!res)
        return NULL;
    size_t j = 0;
    while (j < i)
    {
        res[j] = strdup(target->elt[j]);
        if (!res[j])
        {
            for (size_t k = 0; k < j; k++)
                free(res[k]);
            free(res);
            return NULL;
        }
        j++;
    }
    res[j] = NULL;
    return res;
}

/*Description:
 *  Get the function from the dictionnary
 *Arguments:
 *  key: the function name
 */
struct ast *get_func(struct dictionnary *dict, char *key)
{
    int ind = hash(key);

    struct function *target = dict->function[ind];
    while (target && strcmp(target->key, key) != 0)
    {
        target = target->next;
    }
    if (!target)
    {
        return NULL;
    }
    return target->ast;
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
        free_func(dict->function[i]);
    }
    free(dict);
}
