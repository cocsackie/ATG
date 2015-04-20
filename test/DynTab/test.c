#include<stdlib.h>
#include<stdio.h>
#include<assert.h>

#include "../../DynTab.h"

int main()
{
	int i = 0;
	DynTab * tab = DynTab_create();

	assert(tab->size == 0);

	for( i = 0; i < 10; i++ )
	{
		int * a = malloc(sizeof(int));
		*a = i;
		DynTab_add(tab, a);
	}

	assert(tab->size == 10);
	
	for( i = 0; i < 10; i++ )
	{
		int * a = tab->tab[i];
		assert( *a == i );
	}

	//TODO: add more tests
	DynTab_destroy(tab, free);

	return 0;
}
