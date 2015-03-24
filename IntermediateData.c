#include <stdlib.h>
#include "IntermediateData.h"

DictionaryEntry * DictionaryEntry_create(char * word, int occurences)
{
	DictionaryEntry * dictEntry = malloc (sizeof( DictionaryEntry ) );

	if( dictEntry != NULL )
	{
		dictEntry->word = word;
		dictEntry->occurences = occurences;
	}

	return dictEntry;
}