#ifndef REDIR_EXEC_H
#define REDIR_EXEC_H

#include <stddef.h>

#include "../utils/redir.h"

struct saved_fd
{
    int target_fd;
    int copy_fd;
};

struct redir_saved
{
    struct saved_fd *saved;
    size_t size;
};

int redir_apply(struct redir **redirs, struct redir_saved *redir_saved);
void restore_redirs(struct redir_saved *redir_saved);

#endif /* ! REDIR_EXEC_H */
