#include "redir_exec.h"
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

/* TODO */
static int read_io(char *string, int defaul)
{
	if(!string || !string[0])
		return defaul;
	int res = 0;
	int ind =0;
	while(string[ind])
	{
		if(!isdigit(string[ind]))
		{
			return defaul;
		}
		res = res *10 + (string[ind] - '0');
		ind++;
	}
	return res;
} 

/* TODO */
static int fd_save(struct redir_saved *redir_saved, int fd)
{
	int cpy = dup(fd);
	if(cpy < 0) // fail dup
	{
		return 1;
	}
	struct saved_fd *add_save = realloc(redir_saved->saved, (redir_saved->size +1) * sizeof(struct saved_fd));
	if(!add_save)
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

int redir_apply(struct redir **redirs, struct redir_saved *redir_saved)
{
	redir_saved->saved = NULL;
	redir_saved->size = 0;
	if(!redirs)
	{
		return 0;
	}

	for(size_t i = 0; redirs[i]; i++)
	{
		struct redir *redirect = redirs[i];
		int target = STDOUT_FILENO;
		if(redirect->type == REDIR_IN) // juste > mais à upgrade
			target = STDIN_FILENO;
		target = read_io(redirect->io_num, target);
		if(fd_save(redir_saved, target))
			return 1;
		if(redirect->type == REDIR_OUT) // same
		{
			int fd = open(redirect->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if(fd < 0 )
				return 1;
			if(dup2(fd, target) < 0)
			{
				close(fd);
				return 1;
			}
			close(fd);
		}
		else if(redirect->type == REDIR_IN)
		{
			int fd = open(redirect->target, O_RDONLY);
			if(fd < 0)
				return 1;
			if(dup2(fd, target) < 0)
			{
				close(fd);
				return 1;
			}
			close(fd);
		}
		else
		{
			// not supported
			return 1;
		}
	}
	return 0;
}


void restore_redirs(struct redir_saved *redir_saved)
{
	for(size_t i = redir_saved->size; i > 0; i--) // reversed traversal to restore correctly
	{
		int targ = redir_saved->saved[i-1].target_fd;
		int cpy = redir_saved->saved[i-1].copy_fd;
		dup2(cpy, targ);
		close(cpy);
	}
	free(redir_saved->saved);
	redir_saved->saved = NULL;
	redir_saved->size = 0;
}
