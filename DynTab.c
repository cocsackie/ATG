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

static calcMiddle(int start, int end)
{
	return (start+end)/2;
}

int DynTab_binsearch(DynTab * tab, void * element, DynTabComparator comparator)
{

	int start = 0;
	int end = tab->size-1;

	while( start < end )
	{
		int middle = calcMiddle(start,end); 
		int result = comparator(element, tab->tab[middle]);

		if( result == 0 )
		{
			return middle;
		}
		else if( result > 0 )
		{
			start = middle+1;	
		}
		else if( result < 0 )
		{
			end = middle-1;
		}
	}

	if( comparator(element, tab->tab[start]) == 0 )
	{
		return start;
	}

	return -1;
}
