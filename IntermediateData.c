#include <stdlib.h>
#include <string.h>

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

IntermediateData * IntermediateData_create()
{
	IntermediateData * id = malloc( sizeof(IntermediateData) );
	
	if( id == NULL )
	{
		OutOfMemoryError();
	}

	id->dictionary = DynTab_create();
	id->gramTree = Tree_create( treeInsertComparator );

	return id;
}

void destroyGramTreeEntry(void * value)
{
	GramTreeEntry * entry = value;

	free(entry->prefix);
	DynTab_destroy(entry->suffixes, free);
	free(entry);
}

void freeDictionaryEntry(void * value)
{
	DictionaryEntry * entry = value;

	free(entry->word);
	free(entry);
}

void IntermediateData_destroy(IntermediateData * data)
{
	DynTab_destroy(data->dictionary, freeDictionaryEntry);
	Tree_destroy(data->gramTree, destroyGramTreeEntry);
	free(data);
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
	
	entry->prefix = malloc( sizeof( int ) * (gramSize-1) );

	if( entry->prefix == NULL )
	{
		OutOfMemoryError();
	}

	entry->suffixes = DynTab_create();
	entry->prefixSize = gramSize-1;

	for( i = 0; i < gramSize-1; i++ )
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

GramSuffix * GramSuffix_createEmpty()
{
	return GramSuffix_create(0,0);
}

static FILE * saveFile;

void saveTraverseHandler(void * value)
{
	int i;
	GramTreeEntry * entry = value;
	
	fwrite(entry->prefix, sizeof(int)*entry->prefixSize, 1, saveFile);
	fwrite(&entry->suffixes->size, sizeof(int), 1, saveFile);
	
	for( i = 0; i < entry->suffixes->size; i++ )
	{
		fwrite(entry->suffixes->tab[i], sizeof(GramSuffix), 1, saveFile);
	}
}

char * fileTypePrefix = "ATG_IDATA";

void IntermediateData_save(IntermediateData * data, FILE * file)
{
	int i;
	DynTab * dictionary = data->dictionary;
	saveFile = file;

	fwrite(fileTypePrefix, sizeof(*fileTypePrefix)*strlen(fileTypePrefix), 1, file);

	fwrite(&data->gramType, sizeof(data->gramType), 1, file);
	fwrite(&dictionary->size, sizeof(dictionary->size), 1, file);

	for( i = 0; i < dictionary->size; i++ )
	{
		DictionaryEntry * entry = dictionary->tab[i];
		short wordSize = strlen(entry->word);
		fwrite(&wordSize, sizeof(wordSize), 1, file);
		fwrite(entry->word, sizeof(*entry->word)*wordSize, 1, file);
		fwrite(&entry->occurences, sizeof(entry->occurences), 1, file);
	}

	fwrite(&data->gramTree->size, sizeof(data->gramTree->size), 1, file);

	Tree_traverse(data->gramTree, saveTraverseHandler);
}

static IntermediateData * data;
static DynTab * treeEntriesSorted;

static void createGramTreeInternal(int start, int end)
{	

	int middle = (start+end)/2;

	Tree_insert(data->gramTree, treeEntriesSorted->tab[middle]);

	if( start != middle )
	{
		createGramTreeInternal(start,middle-1);
	}
	if( end != middle )
	{
		createGramTreeInternal(middle+1, end);
	}

}

static void createGramTree()
{
	createGramTreeInternal(0, treeEntriesSorted->size-1);
}

IntermediateData * IntermediateData_load(FILE * file)
{
	int i,j;
	size_t wordCount;
	int grams;
	data = IntermediateData_create();
	treeEntriesSorted = DynTab_create();
	char typePrefix[10];
	int * prefix;
	typePrefix[9] = '\0';
	fread(typePrefix, sizeof(char)*9, 1, file);

	if( strcmp(typePrefix, fileTypePrefix) )
	{
		NotIntermediateFileError();
	}


	fread(&data->gramType, sizeof(data->gramType), 1, file);

	prefix = malloc(sizeof(int) * (data->gramType-1));

	if( prefix == NULL )
	{
		OutOfMemoryError();
	}

	fread(&wordCount, sizeof(wordCount), 1, file);

	for( i = 0; i < wordCount; i++ )
	{
		DictionaryEntry * entry;
		short wordSize;
		int occurences;
		char * word;
		fread(&wordSize, sizeof(wordSize), 1, file);
		
		word = malloc(sizeof(*word)*(wordSize+1));

		if( word == NULL )
		{
			OutOfMemoryError();
		}
		word[wordSize] = '\0';

		fread(word, sizeof(*word)*wordSize, 1, file);
		fread(&occurences, sizeof(occurences), 1, file);

		entry = DictionaryEntry_create(word, occurences);

		DynTab_add(data->dictionary, entry);
	}

	fread(&grams, sizeof(grams), 1, file);

	DynTab_resize(treeEntriesSorted, grams);

	for( i = 0; i < grams; i++ )
	{
		GramTreeEntry * entry;
		int suffixes;
		fread(prefix, sizeof(*entry->prefix)*(data->gramType-1), 1, file);

		entry = GramTreeEntry_create(prefix, data->gramType);

		fread(&suffixes, sizeof(suffixes), 1, file);
		DynTab_resize(entry->suffixes, suffixes);

		for( j = 0; j < suffixes; j++ )
		{
			GramSuffix * suffix = GramSuffix_createEmpty();
			fread(suffix, sizeof(*suffix), 1, file);
			DynTab_add(entry->suffixes, suffix); 
		}
		DynTab_add(treeEntriesSorted, entry);
	}

	createGramTree();

	free(prefix);
	DynTab_destroy(treeEntriesSorted, NULL);
	return data;
}
