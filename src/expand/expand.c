#include "expand.h"

#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"

static int is_word(char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9') || c == '_');
}

static char *var_expand(struct dictionnary *vars, char *word, size_t *ind)
{
    char *key = malloc(1);
    if (!key)
        return NULL;
    key[0] = 0;
    if (word[(*ind) + 1] == '{')
    {
        (*ind) += 2;
        while (word[*ind] != 0 && word[*ind] != '}')
        {
            char *tmp = realloc(key, strlen(key) + 2);
            if (!tmp)
            {
                free(key);
                return NULL;
            }
            key = tmp;
            size_t len = strlen(key);
            key[len] = word[*ind];
            key[len + 1] = 0;
            (*ind)++;
        }
        if (word[*ind] == 0)
        {
            free(key);
            return NULL;
        }
    }
    else
    {
        char next = word[(*ind) + 1];
        if (next == '#' || next == '?' || next == '@' || next == '*'
            || next == '$' || next == '!' || next == '-'
            || (next >= '0' && next <= '9'))
        {
            (*ind)++;
            char *tmp = realloc(key, 2);
            if (!tmp)
            {
                free(key);
                return NULL;
            }
            key = tmp;
            key[0] = next;
            key[1] = 0;
        }
        else
        {
            while (is_word(word[(*ind) + 1]))
            {
                (*ind)++;
                char *tmp = realloc(key, strlen(key) + 2);
                if (!tmp)
                {
                    free(key);
                    return NULL;
                }
                key = tmp;
                size_t len = strlen(key);
                key[len] = word[(*ind)];
                key[len + 1] = 0;
            }
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
        char *tmp = realloc(res, strlen(res) + strlen(val[j]) + 2);
        if (!tmp)
        {
            free(res);
            free_ex(val);
            return NULL;
        }
        res = tmp;
        strcat(res, val[j]);
        if (val[j + 1] != NULL)
            strcat(res, " ");
        free(val[j]);
        j++;
    }
    free(val);
    return res;
}

static int is_expandable(char c)
{
    return (c == '$' || c == '"' || c == '\'' || c == '\\');
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
            if (word[*ind] == '$')
            {
                char *var = var_expand(vars, word, ind);
                if (!var)
                {
                    free(res);
                    return NULL;
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
                (*ind)++;
            }
            else if (word[*ind] == '\\')
            {
                if (is_expandable(word[*ind + 1]))
                {
                    (*ind)++;
                }
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
        res[j] = malloc(1);
        if (!res[j])
        {
            free_ex(res);
            return NULL;
        }
        res[j][0] = 0;
        size_t ibis = 0;
        while (words[i][ibis] != 0)
        {
            if (words[i][ibis] == '$')
            {
                char *var = var_expand(vars, words[i], &ibis);
                if (!var)
                {
                    ibis++;
                    continue;
                }
                char *tmp = realloc(res[j], strlen(res[j]) + strlen(var) + 1);
                if (!tmp)
                {
                    free(var);
                    free_ex(res);
                    return NULL;
                }
                res[j] = tmp;
                strcat(res[j], var);
                free(var);
            }
            else if (words[i][ibis] == '\'')
            {
                char *var = single_quote_expand(words[i], &ibis);
                res[j] = realloc(res[j], strlen(res[j]) + strlen(var) + 1);
                strcat(res[j], var);
                free(var);
            }
            else if (words[i][ibis] == '"')
            {
                char *var = double_quotes_expand(vars, words[i], &ibis);
                res[j] = realloc(res[j], strlen(res[j]) + strlen(var) + 1);
                strcat(res[j], var);
                free(var);
            }
            else if (words[i][ibis] == '\\')
            {
                ibis++;
                res[j] = realloc(res[j], strlen(res[j]) + 2);
                size_t len = strlen(res[j]);
                res[j][len] = words[i][ibis];
                res[j][len + 1] = 0;
            }
            else
            {
                res[j] = realloc(res[j], strlen(res[j]) + 2);
                size_t len = strlen(res[j]);
                res[j][len] = words[i][ibis];
                res[j][len + 1] = 0;
            }
            if (words[i][ibis] != 0)
                ibis++;
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