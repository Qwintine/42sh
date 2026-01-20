#include "io.h"

#include <stdlib.h>
#include <string.h>

// Solution de contournement pour lire stdin avec fmemopen
// (pas sur que ça tienne dans le temps)
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
    return mem;
}

char *itoa(int num)
{
    int i = 0;
    char *res = malloc(20);
    if(num == 0)
    {
        res[0] = '0';
        res[1] = 0;
        return res;
    }

    while(num>0)
    {
        res[i] = (num % 10) + 48;
        num = num / 10;
        i++;
    }

    res[i] = 0;

    for(int j=0; j<i/2; j++)
    {
        char c = res[j];
        res[j] = res[i - j -1];
        res[i - j -1] = c;
    }

    return res;
}

static void arg_num(int num, struct dictionnary *vars)
{
    if (num < 0)
        add_var(vars, "#=0");
    else
    {
        char *a_n = malloc(20);
        a_n[0] = '#';
        a_n[1] = '=';
        a_n[2] = 0;
        char *inum = itoa(num);
        a_n = strcat(a_n, inum);
        free(inum);
        add_var(vars, a_n);
        free(a_n);
    }
}

/* Description:
 * 	Gère options scripts et arguments
 * Arguments:
 * 	Arguments binaire
 * Retour:
 * 	File * -> caractères à parser
 * Verbose:
 * 	-c string
 * 	nom de fichier -> file
 * 	rien -> stdin
 */
FILE *arg_file(int argc, char **argv, int *prettyprint, struct dictionnary *vars)
{
    FILE *entry = NULL;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            arg_num(argc-2, vars);
            i++;
            if (!argv[i])
            {
                fprintf(
                    stderr,
                    "42h: IO no argument after -c\n"); // erreur : pas
                                                       // d'argument après -c
                return NULL;
            }
            entry = fmemopen(argv[i], strlen(argv[i]), "r");
        }
        else if (strcmp(argv[i], "--prettyprint") == 0)
        {
            arg_num(argc-3, vars);
            *prettyprint = 1;
        }
        // autre arguments (pour plus tard)
        else if (!entry)
        {
            arg_num(argc-2, vars);
            entry = fopen(argv[i], "r");
        }
        else
        {
            if(i>1)
            {
                char *arg_name = malloc(20);
                char *inum = itoa(i-1);
                arg_name = strcpy(arg_name, inum);
                free(inum);
                arg_name = strcat(arg_name, "=");
                arg_name = strcat(arg_name, argv[i]);
                add_var(vars, arg_name);
                free(arg_name);
            }
        }
    }
    if(add_var_arg(vars, "@", argv+2))
    {
        return NULL;
    }
    if (!entry)
    {
        entry = stdin_to_mem();
    }
    return entry;
}
