#include "expand.h"

#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"

static int is_word(char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9') || c == '_');
}

static int var_exp_no_brack(char *word, size_t *ind, char **key)
{
    char next = word[(*ind) + 1];
    if (next == '#' || next == '?' || next == '@' || next == '*' || next == '$'
        || next == '!' || next == '-' || (next >= '0' && next <= '9'))
    {
        (*ind)++;
        char *tmp = realloc((*key), 2);
        if (!tmp)
        {
            return 1;
        }
        (*key) = tmp;
        (*key)[0] = next;
        (*key)[1] = 0;
    }
    else
    {
        while (is_word(word[(*ind) + 1]))
        {
            (*ind)++;
            char *tmp = realloc((*key), strlen((*key)) + 2);
            if (!tmp)
            {
                return 1;
            }
            (*key) = tmp;
            size_t len = strlen(*key);
            (*key)[len] = word[(*ind)];
            (*key)[len + 1] = 0;
        }
    }
    return 0;
}

static int var_exp_brack(char *word, size_t *ind, char **key)
{
    (*ind) += 2;
    while (word[*ind] != 0 && word[*ind] != '}')
    {
        char *tmp = realloc(*key, strlen(*key) + 2);
        if (!tmp)
        {
            return 1;
        }
        *key = tmp;
        size_t len = strlen(*key);
        (*key)[len] = word[*ind];
        (*key)[len + 1] = 0;
        (*ind)++;
    }
    if (word[*ind] == 0)
    {
        return 1;
    }
    return 0;
}

static void val_append(char **res, char *to_add, char *next)
{
    char *tmp = realloc(*res, strlen(*res) + strlen(to_add) + 1);
    if (!tmp)
    {
        free(*res);
        *res = NULL;
        return;
    }
    *res = tmp;
    strcat(*res, to_add);
    if (next != NULL)
        strcat(*res, " ");
    free(to_add);
}

static char *var_expand(struct dictionnary *vars, char *word, size_t *ind)
{
    char *key = malloc(1);
    if (!key)
        return NULL;
    key[0] = 0;
    if (word[(*ind) + 1] == '{')
    {
        if (var_exp_brack(word, ind, &key))
        {
            free(key);
            return NULL;
        }
    }
    else
    {
        if (var_exp_no_brack(word, ind, &key))
        {
            free(key);
            return NULL;
        }
    }
    char **val = get_var(vars, key);
    free(key);
    if (!val || !val[0])
    {
        free(val);
        char *empty = malloc(1);
        if (!empty)
            return NULL;
        empty[0] = 0;
        return empty;
    }
    char *res = malloc(1);
    if (!res)
    {
        free_ex(val);
        return NULL;
    }
    res[0] = 0;
    size_t j = 0;
    while (val[j] != NULL)
    {
        val_append(&res, val[j], val[j + 1]);
        j++;
    }
    free(val);
    return res;
}

static int is_expandable(char c)
{
    return (c == '$' || c == '"' || c == '\'' || c == '\\');
}

static int in_dquote_exp(char *word, size_t *ind, struct dictionnary *vars,
                         char **res)
{
    if (word[*ind] == '$')
    {
        char *var = var_expand(vars, word, ind);
        if (!var)
        {
            free(*res);
            return 1;
        }
        char *tmp = realloc(*res, strlen(*res) + strlen(var) + 1);
        if (!tmp)
        {
            free(var);
            free(*res);
            return 1;
        }
        *res = tmp;
        strcat(*res, var);
        free(var);
        (*ind)++;
    }
    else if (word[*ind] == '\\')
    {
        if (is_expandable(word[*ind + 1]))
        {
            (*ind)++;
        }
        char *tmp = realloc(*res, strlen(*res) + 2);
        if (!tmp)
        {
            free(*res);
            return 1;
        }
        *res = tmp;
        size_t len = strlen(*res);
        (*res)[len] = word[*ind];
        (*res)[len + 1] = 0;
        (*ind)++;
    }
    else
    {
        char *tmp = realloc(*res, strlen(*res) + 2);
        if (!tmp)
        {
            free(*res);
            return 1;
        }
        *res = tmp;
        size_t len = strlen(*res);
        (*res)[len] = word[*ind];
        (*res)[len + 1] = 0;
        (*ind)++;
    }
    return 0;
}

static char *double_quotes_expand(struct dictionnary *vars, char *word,
                                  size_t *ind)
{
    char *res = malloc(1);
    if (!res)
        return NULL;
    res[0] = 0;
    int quotes = 0;
    while (quotes < 2)
    {
        if (word[*ind] == 0)
        {
            free(res);
            return NULL;
        }
        if (word[*ind] != '"')
        {
            if (in_dquote_exp(word, ind, vars, &res))
            {
                return NULL;
            }
        }
        else
        {
            (*ind)++;
            quotes++;
        }
    }
    (*ind)--;
    return res;
}

void free_ex(char **ex)
{
    size_t i = 0;
    while (ex[i])
    {
        free(ex[i]);
        i++;
    }
    free(ex);
}

static char *single_quote_expand(char *word, size_t *ind)
{
    char *res = malloc(1);
    if (!res)
        return NULL;
    res[0] = 0;
    int quotes = 0;
    while (quotes < 2)
    {
        if (word[*ind] == 0)
        {
            free(res);
            return NULL;
        }
        if (word[*ind] != '\'')
        {
            char *tmp = realloc(res, strlen(res) + 2);
            if (!tmp)
            {
                free(res);
                return NULL;
            }
            res = tmp;
            size_t len = strlen(res);
            res[len] = word[*ind];
            res[len + 1] = 0;
            (*ind)++;
        }
        else if (word[*ind] == '\\')
        {
            char *tmp = realloc(res, strlen(res) + 2);
            if (!tmp)
            {
                free(res);
                return NULL;
            }
            res = tmp;
            size_t len = strlen(res);
            res[len] = word[*ind];
            res[len + 1] = 0;
            (*ind)++;
        }
        else
        {
            (*ind)++;
            quotes++;
        }
    }
    (*ind)--;
    return res;
}

static int append_char(char **res, char c)
{
    char *tmp = realloc(*res, strlen(*res) + 2);
    if (!tmp)
        return 1;
    *res = tmp;
    size_t len = strlen(*res);
    (*res)[len] = c;
    (*res)[len + 1] = 0;
    return 0;
}

static char *expand_word(struct dictionnary *vars, char *word)
{
    char *res = calloc(1, 1);
    if (!res)
        return NULL;
    size_t ibis = 0;
    while (word[ibis] != 0)
    {
        if (word[ibis] == '$')
        {
            char *var = var_expand(vars, word, &ibis);
            if (!var)
            {
                ibis++;
                continue;
            }
            char *tmp = realloc(res, strlen(res) + strlen(var) + 1);
            if (!tmp)
            {
                free(var);
                free(res);
                return NULL;
            }
            res = tmp;
            strcat(res, var);
            free(var);
        }
        else if (word[ibis] == '\'')
        {
            char *var = single_quote_expand(word, &ibis);
            res = realloc(res, strlen(res) + strlen(var) + 1);
            strcat(res, var);
            free(var);
        }
        else if (word[ibis] == '"')
        {
            char *var = double_quotes_expand(vars, word, &ibis);
            res = realloc(res, strlen(res) + strlen(var) + 1);
            strcat(res, var);
            free(var);
        }
        else if (word[ibis] == '\\')
        {
            ibis++;
            if (append_char(&res, word[ibis]))
            {
                free(res);
                return NULL;
            }
        }
        else
        {
            if (append_char(&res, word[ibis]))
            {
                free(res);
                return NULL;
            }
        }
        if (word[ibis] != 0)
            ibis++;
    }
    return res;
}

char **expand(struct dictionnary *vars, char **words)
{
    char **res = malloc(sizeof(char *));
    if (!res)
        return NULL;
    res[0] = NULL;
    size_t i = 0;
    size_t j = 0;
    while (words[i] != NULL)
    {
        res[j] = expand_word(vars, words[i]);
        if (!res[j])
        {
            free_ex(res);
            return NULL;
        }
        i++;
        j++;
        if (!res[j - 1][0] == 0)
        {
            res = realloc(res, (j + 1) * sizeof(char *));
            res[j] = NULL;
        }
        else
        {
            j--;
            free(res[j]);
            res[j] = NULL;
        }
    }
    if (res[0] == NULL)
    {
        free(res[0]);
        free(res);
        res = malloc(sizeof(char *));
        if (!res)
            return NULL;
        res[0] = NULL;
    }
    return res;
}
