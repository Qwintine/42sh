#ifndef REDIR_H
#define REDIR_H

#include "token.h"

struct redir
{
	char *io_num; // NULL default
	enum type type;
	char *target; // target file
};

struct redir *init_redir();
void free_redir(struct redir *redir);

#endif /* ! REDIR_H */
