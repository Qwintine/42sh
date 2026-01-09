#include "io.h"
#include <string.h>

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
FILE *arg_file(int argc, char** argv, int *prettyprint)
{
	FILE *entry = NULL;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            i++;
            if (!argv[i])
	        {
                fprintf(stderr, "42h: IO no argument after -c\n"); // erreur : pas d'argument après -c
                return NULL;
            }
            entry = fmemopen(argv[i], strlen(argv[i]), "r");
        }
	else if(strcmp(argv[i], "--prettyprint") == 0)
	{
		*prettyprint = 1;
	}
        // autre arguments (pour plus tard)
        else if(!entry)
            entry = fopen(argv[i], "r");
    }
    if(!entry)
    {
	    entry = stdin;
    }
    return entry;
}
