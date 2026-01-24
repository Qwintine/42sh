#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../utils/itoa.h"

enum val_type
{
    VAR,
    FUNC,    
};

struct variables
{
    char *key;
    char **elt;
    struct variables *next;
};

struct function
{
    char *key;
    struct ast *ast;
    struct function *next;
};

struct dictionnary
{
    struct variables *variables[20];
    struct function *function[20];
};

int hash(char *str);
struct dictionnary *init_dict(void);
int is_env(char *key);
int add_var(struct dictionnary *dict, char *varas);
int add_var_arg(struct dictionnary *dict, char *key, char **val);
char **get_var(struct dictionnary *dict, char *key);
void free_var(struct var *val);
void free_func(struct function *function);
void free_dict(struct dictionnary *dict);

#endif /* HASHMAP_H */
