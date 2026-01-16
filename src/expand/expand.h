#ifndef EXPAND_H
#    define EXPAND_H

#    define < stddef.h>
#    define < stdlib.h>

struct values
{
    char *key;
    char **elt;
    struct values *next;
}

struct dictionnary
{
    struct values *values[/*#*/];
    hashing_func *hash;
};

typedef int (*hashing_func)(int);

struct dictionnary *init_dict(hashing_func *h);
int add_var(char *key, char *val, int ind);
void free_val(struct values *val);
void free_dict(struct dictionnary *dict);

#endif /* EXPAND_H */
