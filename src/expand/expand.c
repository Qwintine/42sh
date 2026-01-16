/*Description:
 *	Initialise the dictionnary
 *Argument:
 *	The hashing function used by the dictionnary to hash the key
 */
struct dictionnary *init_dict(hashing_func *h)
{
    struct dictionnary *dict = malloc(sizeof(struct dictionnary));

    dict->hash = h;
    for (size_t i = 0; i < /*#*/; i++)
    {
        dict->values = NULL;
    }

    return dict;
}

/*Description:
 *  Add a variable to the dictionnary
 *Arguments:
 *  key: the variable name
 *  val: the variable value
 *  ind: the result of hash(key)
 */
int add_var(char *key, char *val)
{
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

    int ind = dict->hashing(key);
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
    target->naext = new;
    return 0;
}

char **get_var(char *key)
{
    int ind = dict->hashing(key);

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
    for (size_t i = 0; i < /*#*/; i++)
    {
        free_val(dict->val[i]);
    }
    free(dict);
}
