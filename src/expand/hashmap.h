#ifndef HASHMAP_H
#define HASHMAP_H

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

int hash(char *str);
struct dictionnary *init_dict(void);
int is_env(char *key);
int add_var(struct dictionnary *dict, char *varas);
int add_var_arg(struct dictionnary *dict, char *key, char **val);
char **get_var(struct dictionnary *dict, char *key);
void free_val(struct values *val);
void free_dict(struct dictionnary *dict);

#endif /* HASHMAP_H */
