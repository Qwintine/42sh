#include "unset.h"
#include <stdlib.h>
#include <string.h>

static void free_node(struct variables *val)
{
	if(val)
	{
		free(val->key);
		for(size_t i =0; val->elt[i] ; i++)
			free(val->elt[i]);
		free(val->elt);
		free(val);
	}
}

int unset(struct dictionnary *to_unset, char **names)
{
	int cant_del = 0; // useless right now but later for readonly vars
	if(to_unset && names)
	{
		for(size_t i =0; names[i]; i++) // maybe use better algo ?
		{
			int ind = hash(names[i]);
			struct variables *val = to_unset->variables[ind];//use hash fonc]
			if(val)
			{
				if(!strcmp(val->key, names[i]))
				{
					if(!val->next)
					{
						to_unset->variables[ind] = NULL;
						free_node(val);
					}
					else
					{
						to_unset->variables[ind] = val->next;
						free_node(val);
					}
				}
				else
				{
					while(val && val->next && strcmp(val->next->key, names[i]))
					{
						val = val->next;
					}
					if(val && val->next) // possible to add a check for readonly once implem
					{
						struct variables *temp = val->next;
						val->next = temp->next;
						free_node(temp);
					}
				}
			}
		}
	}
	return cant_del;
} 
