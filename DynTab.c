#include <stdlib.h>
#include <string.h>

#include "DynTab.h"
#include "Debug.h"

const size_t defaultSize = 16;

DynTab * DynTab_create()
{
	DynTab * tab = malloc( sizeof(DynTab) );
	
	if( tab != NULL )
	{
		tab->size = 0;
		tab->capacity = defaultSize;
		tab->tab = malloc( sizeof( *tab->tab ) * defaultSize );
		if( tab->tab == NULL )
		{
			free( tab );
		} 
	}

	return tab;
}

boolean DynTab_add(DynTab * tab, void * element)
{
	assert(tab != NULL);
	
	if( tab->size == tab->capacity )
	{
		void * tmp = realloc(tab->tab, tab->capacity*2*sizeof(void *));
		if( tmp == NULL )
		{
			return FALSE;
		}
		tab->tab = tmp;
		tab->capacity *= 2;
	}

	assert( tab->size < tab->capacity );
	
	tab->tab[tab->size] = element;
	tab->size++;
	return TRUE;
}
