#define _POSIX_C_SOURCE 200809L
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "dot.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static char *concat(char *path, char *string)
{
	char *res = strdup(path);
	if(!res)
		return NULL;
	size_t size = strlen(res);
	char *tmp = realloc(res, size +2);
	if (!tmp) 
	{
		free(res);
		return NULL;
	}

	res = tmp;
	res[size++] = '/';
	res[size] = '\0';
	
	if(string)
	{
		for(size_t i= 0; string[i]; i++)
		{
			tmp = realloc(res,size +2);
			if(!tmp)
			{
				free(res);
				return NULL;
			}
			res = tmp;
			res[size++] = string[i];
			res[size] = '\0';
		}
	}
	return res;
}

int dot_b(char **words, struct dictionnary *vars, int *exit)
{
	if(*exit)
		return 1;
	
	if (!words || !words[0])
	{
		fprintf(stderr, "42sh: dot: filename argument required\n");
		return 2;
	}

	FILE *entry = NULL;
	
	if(strchr(words[0], '/'))
		entry = fopen(words[0], "r"); // / in file path
	else // search trough PATH pafs values
	{
		char *path = get_var(vars, "PATH")[0];
		if(!path)
			path =""; // empty paf -> search in current dir
		while(!entry && *path)
		{
			char *semi_colon = strchr(path, ':');
			size_t size_path;
			if(semi_colon)
				size_path = semi_colon-path;
			else
				size_path = strlen(path);

			char *string = NULL;

			if (size_path == 0) // empty path -> current dir
			{
				string = strdup("."); 
				if (!string)
					break;
			}
			else 
			{
				string = malloc(size_path + 1);
				if (!string) 
					break;
				memcpy(string, path, size_path);
				string[size_path] = '\0';
			}

			char *full = concat(string, words[0]);
			free(string);

			if (!full) 
			{
				break;
			}

			entry = fopen(full, "r");
			free(full);

			if (!semi_colon)
				break;

			path = semi_colon + 1;
		}
	}
	if(!entry)
	{
		fprintf(stderr, "42sh: dot: error fopen\n");
		*exit =1;
		return 1;
	}

	int eof = 0;
	int res = 0;

	while (!eof)
	{
		struct ast *ast = parser(entry, &eof);

		if (!ast)
		{
			fclose(entry);
			fprintf(stderr, "42sh: dot: grammar/syntax error\n");
			return 2;
		}
		else
		{
			if (ast->type != AST_LIST
					|| ((struct ast_list *)ast)->elt != NULL)
			res = run_ast(ast, vars, exit); // derniere valeur de retour
			if (*exit)
			{
				free_ast(ast);
				break;
			}
		}

		free_ast(ast);
	}

	fclose(entry);
	return res;
}
