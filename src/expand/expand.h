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
    struct values *values[20];
};

typedef int (*hashing_func)(int);

struct dictionnary *init_dict(hashing_func *h);
int is_env(char *key) int add_var(char *key, char *val, int ind);
char **get_var(char *key) void free_val(struct values *val);
void free_dict(struct dictionnary *dict);

#endif /* EXPAND_H */
