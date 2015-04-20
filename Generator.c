#include<stdlib.h>

#include"Generator.h"
#include"IntermediateData.h"
#include"DynTab.h"

static int randomNode;
static GramTreeEntry * randomEntry;

static int currentWord;

static void randomSelectTraverser(void * value)
{
	GramTreeEntry * entry = (GramTreeEntry*) value;

	if( randomNode == 0 && randomEntry == NULL )
	{
		randomEntry = entry;
	}
	else if( randomNode != 0 )
	{
		randomNode--;	
	}
}

static int treeFindNextEntry(const void * a, const void * b)
{
	int i;
	GramTreeEntry * currentEntry = (GramTreeEntry*) a;
	GramTreeEntry * entry = (GramTreeEntry*) b;

	for( i = 0; i < entry->prefixSize-1; i++ )
	{
		if( currentEntry->prefix[i+1] != entry->prefix[i] )
		{
			return currentEntry->prefix[i] < entry->prefix[i] ? -1 : 1;
		}
	}

	if( currentWord != entry->prefix[entry->prefixSize-1] )
	{
		return currentWord < entry->prefix[i] ? -1 : 1;
	}

	return 0;
}

static GramTreeEntry * getNextEntry(IntermediateData * intermediateData, GramTreeEntry * entry)
{
	return Tree_find(intermediateData->gramTree, entry, treeFindNextEntry);
}

static GramTreeEntry * getRandomEntry(IntermediateData* intermediateData)
{
	randomEntry = NULL;
	randomNode = rand()%(intermediateData->gramTree->size);

	Tree_traverse(intermediateData->gramTree, randomSelectTraverser);

	return randomEntry;
}

static int getRandomSuffix(GramTreeEntry * entry)
{
	DynTab * suffixes = entry->suffixes;
	int i,j;
	int size = suffixes->size;
	int events = 0;
	int r;
	GramSuffix * suffix;

	for( i = 0; i < size; i++ )
	{
		suffix = suffixes->tab[i];
		events += suffix->occurences;
	}
	
	r = rand()%events;

	for( i = 0; i < size; i++ )
	{
		suffix = suffixes->tab[i];
		for( j = 0; j < suffix->occurences; j++ )
		{
			if( r == 0 )
			{
				break;
			}
			else
			{
				r--;
			}
		}
		if( r == 0 )
		{
			break;
		}
	}

	return suffix->word;

}


void Generator_generate(IntermediateData * intermediateData, FILE * file, int words, int paragraphs)
{
	int i,j;
	GramTreeEntry * entry = NULL;
	srand(time(NULL));	

	for( i = 0; i < paragraphs; i++ )
	{
		for( j = 0; j < words; j++ )
		{
			if( entry != NULL )
			{
				entry = getNextEntry(intermediateData, entry);
				
				if( entry != NULL )
				{			
					currentWord = getRandomSuffix(entry);
					fprintf(file, "%s ", ((DictionaryEntry*)intermediateData->dictionary->tab[currentWord])->word);
				}
			}
			if( entry == NULL )
			{
				int k;
				entry = getRandomEntry(intermediateData);

				for( k = 0; j < words && k < entry->prefixSize; j++, k++ )
				{	
					fprintf(file, "%s ", ((DictionaryEntry*)intermediateData->dictionary->tab[entry->prefix[k]])->word);
				}
				j--;
			}
		}
		fprintf(file, "\n\n");
		entry = NULL;
	}

}
