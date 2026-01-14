
#include "redir.h"

struct redir *init_redir(enum redir_type)
{
	struct redir *redir = malloc(sizeof(struct redir));
	if(!redir)
	{
		return NULL;
	}
	redir->io_num = NULL;
	redir->target = NULL;
}

void free_redir(struct redir *redir)
{

}
