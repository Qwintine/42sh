#include "redir.h"

#include <stdlib.h>

/**
 * @brief Init redir struct
 * 
 * @return struct redir * -> inited struct
 */
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

/**
 * @brief Free redir struct
 * 
 * @param struct redir *redir -> redir to free
 */
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
