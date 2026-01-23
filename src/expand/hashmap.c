#define _POSIX_C_SOURCE 200809L
#include "expand.h"

static int hash(char *str)
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
        dict->values[i] = NULL;
    }

    return dict;
}

int is_env(char *key)
{
    return !strcmp(key, "PATH") || !strcmp(key, "HOME") || !strcmp(key, "PWD") || !strcmp(key, "$");
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
    if (!strcmp(key, "$") || !strcmp(key, "PWD")|| !strcmp(key, "HOME"))
    {
        char *r = getenv(key);
        return strdup(r);
    }
    return 0;
}

static int update_or_append_var(struct values *bucket, struct values *new,
                                char *key, char *val)
{
    struct values *target = bucket;
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
    if (is_env(key))
    {
        if (setenv(key, val, 1) != 0)
        {
            goto ERROR;
        }
    }

    struct values *new = malloc(sizeof(struct values));

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
    if (!dict->values[ind])
    {
        dict->values[ind] = new;
        return 0;
    }

    return update_or_append_var(dict->values[ind], new, key, val);

ERROR:
    free(key);
    free(val);
    return 1;
}

int add_var_arg(struct dictionnary *dict, char *key, char **val)
{
    struct values *new = malloc(sizeof(struct values));

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
    if (!dict->values[ind])
    {
        dict->values[ind] = new;
        return 0;
    }

    struct values *target = dict->values[ind];
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

    struct values *target = dict->values[ind];
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

void free_val(struct values *val)
{
    while (val)
    {
        struct values *last = val;
        val = val->next;
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

void free_dict(struct dictionnary *dict)
{
    for (size_t i = 0; i < 20; i++)
    {
        free_val(dict->values[i]);
    }
    free(dict);
}
