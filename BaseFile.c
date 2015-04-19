#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "IntermediateData.h"
#include "Tree.h"
#include "Error.h"
#include "Util.h"

static IntermediateData * data;
static DynTab * dictionary;

static char word[2048];

static void dictionaryTreeTraverseHandler(void * value)
{
	char * str = (char *) value;

	if( dictionary->size == 0 )
	{
		DictionaryEntry * entry = DictionaryEntry_create(copyString(str), 1);
		DynTab_add(dictionary, entry);
	}
	else
	{
		if( strcmp(((DictionaryEntry*)dictionary->tab[dictionary->size-1])->word, str) )
		{
			DictionaryEntry * entry = DictionaryEntry_create(copyString(str), 1);
	                DynTab_add(dictionary, entry);
		}
		else
		{
			((DictionaryEntry*)dictionary->tab[dictionary->size-1])->occurences++;
		}
	}
}

static void createDictionary( DynTab * files, int gramType )
{
	int i;
	Tree * tree = Tree_create( (TreeComparator) strcmp );
	dictionary = DynTab_create();

	for( i = 0; i < files->size; i++ )
	{
		int r;

		while( (r = fscanf(files->tab[i], "%s", word)) != EOF )
		{
			Tree_insert(tree, copyString(word));	
		}
	}

	Tree_traverse( tree, (TreeTraverseHandler) dictionaryTreeTraverseHandler );
	Tree_destroy( tree, (TreeNodeDestructor) free );
	
	DynTab_destroy(data->dictionary, free);
	data->dictionary = dictionary;
}

static int gramTreeComparator( const void * a, const void * b )
{
	int i;
	int * lastWords = (int*) a;
	GramTreeEntry * entry = (GramTreeEntry*) b;
	int * prefix = entry->prefix;

	for( i = 0; i < entry->prefixSize; i++ )
	{
		if(lastWords[i] != prefix[i])
		{
			return lastWords[i] < prefix[i] ? -1 : 1;
		}
	}

	return 0;
}

static int suffixSearchComparator(const void * a, const void * b)
{
	int lastWordSuffix = *((int *) a);
	GramSuffix * gramSuffix = (GramSuffix*) b;

	if( lastWordSuffix == gramSuffix->word )
	{
		return 0;
	}

	return lastWordSuffix < gramSuffix->word ? -1 : 1;
}

static void expandGramTreeWithFile( FILE * file, int gramType )
{
	int i,r,count = 0;
	int * lastWords = malloc( sizeof(int) * gramType );

	if( lastWords == NULL )
	{
		OutOfMemoryError();
	}

	while( (r = fscanf(file, "%s", word)) != EOF )
	{
		int wordIndex = DynTab_binsearch(dictionary, word, dictionaryWordCompare);

		if( count < gramType-1 )
		{
			lastWords[count++] = wordIndex;
		}
		else
		{
			lastWords[gramType-1] = wordIndex;

			GramTreeEntry * entry = Tree_find(data->gramTree, lastWords, (TreeComparator) gramTreeComparator);		

			if(entry == NULL)
			{
				GramTreeEntry * newEntry = GramTreeEntry_create(lastWords, gramType);
				GramSuffix * newSuffix = GramSuffix_create(lastWords[gramType-1], 1); 
				Tree_insert(data->gramTree, newEntry);
				DynTab_add(newEntry->suffixes, newSuffix);
			}
			else
			{
				int suffixIndex = DynTab_search(entry->suffixes, &lastWords[gramType-1], (DynTabComparator) suffixSearchComparator);

				if( suffixIndex == -1 )
				{
					GramSuffix * newSuffix = GramSuffix_create(lastWords[gramType-1], 1);
					DynTab_add(entry->suffixes, newSuffix);
				}
				else
				{
					GramSuffix * suffix = entry->suffixes->tab[suffixIndex];
					suffix->occurences++;
				}
			}

			for( i = 0; i < gramType-1; i++ )
			{
				lastWords[i] = lastWords[i+1];
			}
		}
	}
	free(lastWords);
}

static int diffGrams;

static void debugTraverser( void * value )
{
	int i,j;
	GramTreeEntry * e = (GramTreeEntry*) value;

	for( i = 0; i < e->suffixes->size; i++ )
	{
		printf("N-GRAM: ");
		for( j = 0; j < e->prefixSize; j++ )
		{
			printf("%s,", ((DictionaryEntry*)data->dictionary->tab[e->prefix[j]])->word);
		}
		printf("%s\n", ((DictionaryEntry*)data->dictionary->tab[((GramSuffix*)e->suffixes->tab[i])->word])->word);
	}
	diffGrams += e->suffixes->size; 
}

static void createGramTree( DynTab * files, int gramType )
{
	int i;

	for( i = 0; i < files->size; i++ )
	{
		expandGramTreeWithFile(files->tab[i], gramType);
	}

	//debug

	//Tree_traverse(data->gramTree, (TreeTraverseHandler) debugTraverser);
	//printf("Wczytano %d roznych gramow!\n", diffGrams);

	//debug-end
}

IntermediateData * BaseFile_loadBaseFilesToIntermediateData(DynTab * files, int gramType)
{
	int i;
	data = IntermediateData_create();
	
	data->gramType = gramType;

	createDictionary( files, gramType );

	for( i = 0; i < files->size; i++ )
	{
		rewind(files->tab[i]);
	}

	createGramTree( files, gramType );

	return data;
}
