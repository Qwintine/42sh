#include "redir.h"

#include <stdlib.h>

struct redir *init_redir(void)
{
    struct redir *redir = malloc(sizeof(struct redir));
    if (!redir)
    {
        return NULL;
    }
    redir->io_num = NULL;
    redir->target = NULL;
    redir->type = REDIR_OUT;
    return redir;
}

void free_redir(struct redir *redir)
{
    if (redir->io_num)
    {
        free(redir->io_num);
    }
    if (redir->target)
    {
        free(redir->target);
    }
    free(redir);
}
