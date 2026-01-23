#ifndef EXPAND_H
#define EXPAND_H

#include "hashmap.h"

char **expand(struct dictionnary *vars, char **words);
void free_ex(char **ex);

#endif /* EXPAND_H */
