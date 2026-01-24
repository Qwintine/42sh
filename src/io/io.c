#include "io.h"

#include <stdlib.h>
#include <string.h>

static char *stdin_buffer = NULL;

void free_stdin_buffer(void)
{
    if (stdin_buffer)
    {
        free(stdin_buffer);
        stdin_buffer = NULL;
    }
}

/* Description:
 * 	Charge stdin en mémoire via fmemopen
 * Arguments:
 * 	void
 * Retour:
 * 	FILE * -> pointeur sur buffer mémoire contenant stdin
 * 	NULL en cas d'erreur d'allocation
 * Verbose:
 * 	Lit stdin par blocs de 4096 octets
 * 	Double la capacité du buffer si nécessaire
 * 	Le buffer alloué est stocké dans stdin_buffer (à libérer via free_stdin_buffer)
 */
static FILE *stdin_to_mem(void)
{
    size_t capacity = 4096;
    size_t size = 0;
    char *buffer = malloc(capacity);
    if (!buffer)
        return NULL;

    size_t n = 0;
    while ((n = fread(buffer + size, 1, capacity - size, stdin)) > 0)
    {
        size += n;
        if (size == capacity)
        {
            capacity *= 2;
            char *new_buffer = realloc(buffer, capacity);
            if (!new_buffer)
            {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
    }

    FILE *mem = fmemopen(buffer, size, "r");
    if (!mem)
    {
        free(buffer);
        return NULL;
    }
    stdin_buffer = buffer;
    return mem;
}

static void arg_num(int num, struct dictionnary *vars)
{
    if (vars == NULL)
        return;
    if (num < 0)
        add_var(vars, "#=0");
    else
    {
        char *a_n = malloc(20);
        if (!a_n)
            return;
        a_n[0] = '#';
        a_n[1] = '=';
        a_n[2] = 0;
        char *inum = itoa(num);
        if (!inum)
        {
            free(a_n);
            return;
        }
        a_n = strcat(a_n, inum);
        free(inum);
        add_var(vars, a_n);
        free(a_n);
    }
}


/* Description:
 *  Helper: Ajoute un arg au dictionnaire de variables
 * Arguments:
 * 	vars -> dictionnaire de variables
 * 	index -> numéro de l'arg
 * 	value -> valeur de l'arg
 * Retour:
 * 	void
 */
static void add_positional_arg(struct dictionnary *vars, int index, 
                               char *value)
{
    char *arg_name = malloc(20);
    if (!arg_name)
        return;
    
    char *inum = itoa(index);
    if (!inum)
    {
        free(arg_name);
        return;
    }
    
    strcpy(arg_name, inum);
    free(inum);
    strcat(arg_name, "=");
    strcat(arg_name, value);
    add_var(vars, arg_name);
    free(arg_name);
}


/* Description:
 * 	Gère options scripts et arguments
 * Arguments:
 * 	argc, argv -> arguments passés au programme
 * 	prettyprint -> int à 1 si option --prettyprint passée
 * 	vars -> dictionnaire de variables
 * Retour:
 * 	File * -> caractères à parser
 * Verbose:
 * 	-c string
 * 	nom de fichier -> file
 * 	rien -> stdin
 */
FILE *arg_file(int argc, char **argv, int *prettyprint,
               struct dictionnary *vars)
{
    FILE *entry = NULL;
    int arg_count = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            arg_count = 3;
            i++;
            if (!argv[i])
            {
                // erreur : pas d'argument après -c
                fprintf(stderr,"42h: IO no argument after -c\n"); 
                return NULL;
            }
            entry = fmemopen(argv[i], strlen(argv[i]), "r");
            if (!entry)
            {
                return NULL;
            }
        }
        else if (strcmp(argv[i], "--prettyprint") == 0)
        {
            arg_count--;
            *prettyprint = 1;
        }
        // autre arguments (pour plus tard)
        else if (!entry)
        {
            arg_count = 2;
            entry = fopen(argv[i], "r");
            if (!entry)
            {
                return NULL;
            }
        }
        else if (i > 1 && vars != NULL)
        {
            add_positional_arg(vars, i - arg_count, argv[i]);
        }
    }
    arg_num(argc - arg_count, vars);
    add_var_arg(vars, "@", argv + arg_count);
    if (!entry)
    {
        entry = stdin_to_mem();
    }
    return entry;
}
