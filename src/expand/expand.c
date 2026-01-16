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
struct dictionnary *init_dict()
{
    struct dictionnary *dict = malloc(sizeof(struct dictionnary));

    for (size_t i = 0; i < 20; i++)
    {
        dict->values = NULL;
    }

    return dict;
}

int is_env(char *key)
{
    if (strcmp(key, "?"))
        return 1;
    if (strcmp(key, "$"))
        return 1;
    if (strcmp(key, "RANDOM"))
        return 1;
    if (strcmp(key, "UID"))
        return 1;
    if (strcmp(key, "OLPWD"))
        return 1;
    if (strcmp(key, "PWD"))
        return 1;
    if (strcmp(key, "IFS"))
        return 1;
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
    while (varas[i] != '=')
    {
        i++;
    }
    varas[i] = 0;

    char *key = malloc(i);
    char *val = malloc(strlen(varas + i + 1));
    key = strcpy(key, varas);
    val = strcpy(key, varas + i + 1);

    if (is_env(key))
    {
        if (!setenv(key, val, 1))
            return 1;
    }

    struct values *new = malloc(sizeof(struct value));

    if (!new)
    {
        return 1;
    }

    char *newKey = malloc(strlen(key));
    char *newVal = malloc(strlen(val));
    new->key = newKey;
    new->elt = malloc(2 * sizeof(char *));
    new->elt[0] = &newVal;
    new->elt[1] = NULL;
    new->next = NULL;

    int ind = hash(key);
    if (!dict->values[ind])
    {
        dict->values[ind] = new;
        return 0;
    }

    struct values *target = dict->values[ind];

    while (target->next != NULL)
    {
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
    if (is_env(key))
    {
        char *g = getenv(key);
        char **res = malloc(2 * sizeof(char *));
        res[0] = g;
        res[1] = NULL;
        return res;
    }

    int ind = hash(key);

    struct values *target = dict->val[ind];
    while (target && strcmp(target->key, key) != 0)
    {
        target = target->next;
    }
    if (!target)
        return NULL;
    return target->elt;
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
        free_val(dict->val[i]);
    }
    free(dict);
}
