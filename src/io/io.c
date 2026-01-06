#include "io.h"
#include <string.h>

// -c string
// nom de fichier -> file
// rien -> stdin

FILE *arg_file(char** argv, int argc)
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            i++;
            if (!argv[i])
                return NULL; // erreur : pas d'argument après -c
            return fmemopen(argv[i], strlen(argv[i]), "r");
        }
        // autre arguments (pour plus tard)
        else
            return fopen(argv[i], "r");
    }
    return stdin;
}
