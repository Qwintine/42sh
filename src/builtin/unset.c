#include "../expand/expand.h"
#include "unset.h"
#include <string.h>
int unset(struct dictionnary *to_unset, char **names)
{
	int cant_del = 0; // useless right now but later for readonly vars
	if(to_unset && names)
	{
		for(size_t i =0; names[i]; i++) // maybe use better algo ?
		{
			struct values *val = to_unset->values[hash(names[i])];//use hash fonc]
			if(val)
			{
				if(!strcmp(val->key, names[i]))
				{
					if(!val->next)
					{
						free(val);
					}
					else
					{
						struct values *temp = val->next;
						val->next = temp->next;
						free(temp);
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
						struct values *temp = val->next;
						val->next = temp->next;
						free(temp);
					}
				}
			}
		}
	}
	return cant_del;
} 
