#include "redir_exec.h"

#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/* Description:
 * 	Read io num from string
 * Arguments:
 * 	char *string -> string to read
 * 	int defaul -> default value
 * Return:
 *     int -> io num, defaul if invalid
 */
static int read_io(char *string, int defaul)
{
    if (!string || !string[0])
        return defaul;
    int res = 0;
    int ind = 0;
    while (string[ind])
    {
        if (!isdigit(string[ind]))
        {
            return defaul;
        }
        res = res * 10 + (string[ind] - '0');
        ind++;
    }
    return res;
}

/* Description:
 * 	Save fd state
 * Arguments:
 * 	struct redir_saved *redir_saved -> struct to save fds
 * 	int fd -> fd to save
 * Return:
 * 	int -> 0 success, 1 failure
 */
static int fd_save(struct redir_saved *redir_saved, int fd)
{
    if (fcntl(fd, F_GETFD) < 0) // no fd open
    {
        return 0;
    }

    int cpy = dup(fd);
    if (cpy < 0) // fail dup
    {
        return 1;
    }
    struct saved_fd *add_save = realloc(
        redir_saved->saved, (redir_saved->size + 1) * sizeof(struct saved_fd));
    if (!add_save)
    {
        close(cpy);
        return 1;
    }
    redir_saved->saved = add_save;
    redir_saved->saved[redir_saved->size].target_fd = fd;
    redir_saved->saved[redir_saved->size].copy_fd = cpy;
    redir_saved->size++;
    return 0;
}

/* Description:
 * 	Open file for redir
 * Arguments:
 * 	enum type type -> redir type
 * 	char *target -> targ file
 * Return:
 * 	int -> fd, -1 failure
 */
static int open_redir_file(enum type type, char *target)
{
    if (type == REDIR_OUT || type == REDIR_NO_CLOBB)
        return open(target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (type == REDIR_APPEND)
        return open(target, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (type == REDIR_IO)
        return open(target, O_RDWR | O_CREAT, 0644);
    if (type == REDIR_IN)
        return open(target, O_RDONLY);
    return -1;
}

/* Description:
 * 	Handle dup redir
 * Arguments:
 * 	struct redir *redirect -> redir struct
 * 	int target -> target fd
 * Return:
 * 	int ->0 success, 1 failure
 */
static int handle_dup_redir(struct redir *redirect, int target)
{
    int source_fd = read_io(redirect->target, -1);
    if (source_fd < 0 || fcntl(source_fd, F_GETFD) < 0)
        return 1;
    if (dup2(source_fd, target) < 0)
        return 1;
    return 0;
}

/* Description:
 * 	Apply redirs
 * Arguments:
 * 	struct redir **redirs -> redir to apply
 * 	struct redir_saved *redir_saved -> struct save fd
 * Return:
 * 	int -> 0 success, 1 failure
 */
int redir_apply(struct redir **redirs, struct redir_saved *redir_saved)
{
    redir_saved->saved = NULL;
    redir_saved->size = 0;
    if (!redirs)
        return 0;

    for (size_t i = 0; redirs[i]; i++)
    {
        struct redir *redirect = redirs[i];
        int target = STDOUT_FILENO;
        if (redirect->type == REDIR_IN || redirect->type == REDIR_DUP_IN
            || redirect->type == REDIR_IO)
            target = STDIN_FILENO;

        target = read_io(redirect->io_num, target);
        if (fd_save(redir_saved, target))
            return 1;

        if (redirect->type == REDIR_DUP_OUT || redirect->type == REDIR_DUP_IN)
        {
            if (handle_dup_redir(redirect, target))
                return 1;
            continue;
        }

        int fd = open_redir_file(redirect->type, redirect->target);
        if (fd < 0)
            return 1;

        if (dup2(fd, target) < 0)
        {
            close(fd);
            return 1;
        }
        close(fd);
    }
    return 0;
}

/* Description:
 * 	Restore saved fds
 * Arguments:
 * 	struct redir_saved *redir_saved -> struct with saved fds
 */
void restore_redirs(struct redir_saved *redir_saved)
{
    for (size_t i = redir_saved->size; i > 0;
         i--) // reversed traversal to restore correctly
    {
        int targ = redir_saved->saved[i - 1].target_fd;
        int cpy = redir_saved->saved[i - 1].copy_fd;
        dup2(cpy, targ);
        close(cpy);
    }
    free(redir_saved->saved);
    redir_saved->saved = NULL;
    redir_saved->size = 0;
}
