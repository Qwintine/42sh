#include "io.h"

#include <stdlib.h>
#include <string.h>

// Solution de contournement pour lire stdin avec fmemopen
// (pas sur que ça tienne dans le temps)
static FILE *stdin_to_mem(char **buff)
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
    *buff = buffer;
    return mem;
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
FILE *arg_file(int argc, char **argv, int *prettyprint, char **buff)
{
    FILE *entry = NULL;
    *buff = NULL;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
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
            *prettyprint = 1;
        }
        // autre arguments (pour plus tard)
        else if (!entry)
            entry = fopen(argv[i], "r");
    }
    if (!entry)
    {
        entry = stdin_to_mem(buff);
    }
    return entry;
}
