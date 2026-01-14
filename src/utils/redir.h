#ifndef REDIR_H
#define REDIR_H

enum redir_type
{
	R_OUT, // >
	R_NO_CLOBB, // >| 
	R_IN, // < 
	R_APPEND, // >>
	R_DUP_OUT, // >&
	R_DUP_IN, // <&
	R_IO // <>
};

struct redir 
{
	int io_num; // 0 present, -1 default
	enum redir_type type;
	char *word; // target file
};


#endif /* ! REDIR_H */
