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
FILE *arg_file(int argc, char** argv)
{
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
 	    
            return fmemopen(argv[i], strlen(argv[i]), "r");
        }
        // autre arguments (pour plus tard)
        else
            return fopen(argv[i], "r");
    }
    return stdin;
}
