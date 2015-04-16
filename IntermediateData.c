#include <stdlib.h>
#include "IntermediateData.h"
#include "Error.h"
#include "Util.h"
#include "Debug.h"

static int treeInsertComparator(const void * a, const void * b)
{
	int i;
	GramTreeEntry *pa = (GramTreeEntry*) a;
	GramTreeEntry *pb = (GramTreeEntry*) b;

	int * ta = pa->prefix;
	int * tb = pb->prefix;

	assert( pa->prefixSize == pb->prefixSize );

	for( i = 0; i < pa->prefixSize; i++ )
	{
		if(ta[i] != tb[i])
		{
			return ta[i] < tb[i] ? -1 : 1;
		}
	}

	return 0;
}

IntermediateData * IntermediateData_create( int gramType )
{
	IntermediateData * id = malloc( sizeof(IntermediateData) );
	
	if( id == NULL )
	{
		OutOfMemoryError();
	}

	id->dictionary = DynTab_create();
	id->gramTree = Tree_create( treeInsertComparator );
	id->gramType = gramType;

	return id;
}

DictionaryEntry * DictionaryEntry_create(char * word, int occurences)
{
	DictionaryEntry * dictEntry = malloc (sizeof( DictionaryEntry ) );

	if( dictEntry == NULL )
	{
		OutOfMemoryError();
	}

	dictEntry->word = word;
	dictEntry->occurences = occurences;

	return dictEntry;
}

GramTreeEntry * GramTreeEntry_create(int * prefix, int gramSize)
{
	int i;
	GramTreeEntry * entry = malloc( sizeof( GramTreeEntry ) );
	
	if( entry == NULL )
	{
		OutOfMemoryError();
	}
	
	entry->prefix = malloc( sizeof( int ) * gramSize );

	if( entry->prefix == NULL )
	{
		OutOfMemoryError();
	}

	entry->suffixes = DynTab_create();
	entry->prefixSize = gramSize-1;

	for( i = 0; i < gramSize; i++ )
	{
		entry->prefix[i] = prefix[i];
	}

	return entry;
}

int dictionaryWordCompare(const void * a, const void * b)
{
	char * pa = (char *) a;
	DictionaryEntry * pb = (DictionaryEntry*) b;

	return strcmp(a, pb->word);
}

GramSuffix * GramSuffix_create(int word, int occurences)
{
	GramSuffix * suffix = malloc( sizeof(GramSuffix) );
	
	if(suffix == NULL)
	{
		OutOfMemoryError();
	}

	suffix->word = word;
	suffix->occurences = occurences;

	return suffix;	
}
