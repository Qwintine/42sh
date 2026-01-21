#ifndef EXPAND_H
#define EXPAND_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct values
{
    char *key;
    char **elt;
    struct values *next;
};

struct dictionnary
{
    struct values *values[20];
};

typedef int (*hashing_func)(int);

struct dictionnary *init_dict(void);
int is_env(char *key);
int add_var(struct dictionnary *dict, char *varas);
int add_var_arg(struct dictionnary *dict, char *key, char **val);
char **get_var(struct dictionnary *dict, char *key);
void free_val(struct values *val);
void free_dict(struct dictionnary *dict);

#endif /* EXPAND_H */
