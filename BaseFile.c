#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"TypesAndDefs.h"
#include"BaseFile.h"

struct Trie
{
	struct Trie * next[256];
	int cnt;
};

typedef struct Trie Trie;

static DynTab * dictionary;
static DynTab * dictWithMultipleEntries;
static char word[2048];

static char * copyString(const char * str)
{
	char * copyStr = malloc( strlen( str ) + 1 );
	strcpy( copyStr, str );

	return copyStr;
}

static boolean readFile(const char * fileName)
{
	FILE * file = fopen(fileName, "r");
	int r;

	while( (r = fscanf(file, "%s", word)) != EOF )
	{
		boolean added = DynTab_add(dictWithMultipleEntries, copyString(word));
	}
	
	fclose(file);	
}

int cmpstr(const void *a, const void * b)
{
	const char *ap = *(const char **)a;
	const char *bp = *(const char **)b;

	return strcmp(ap, bp);
}

struct IntermediateData * BaseFile_loadBaseFilesToIntermediateData(DynTab * fileNames)
{
	int i;
	dictionary = DynTab_create();
	dictWithMultipleEntries = DynTab_create();
	for( i = 0; i < fileNames->size; i++ )
	{
		readFile( (char *)fileNames->tab[i] );
	}

	for( i = 0; i < dictWithMultipleEntries->size; i++ )
	{
		if( dictWithMultipleEntries->tab[i] == NULL )
		{
			printf("Index: %d", i );
			abort();
		}
	}
	
	qsort( dictWithMultipleEntries->tab, dictWithMultipleEntries->size, sizeof(dictWithMultipleEntries->tab[0]), cmpstr );
	
	for( i = 0; i < dictWithMultipleEntries->size; i++ )
	{
		int start = i;
		DynTab_add(dictionary, dictWithMultipleEntries->tab[i]);
		i++;
		while(i < dictWithMultipleEntries->size)
		{
			if(strcmp(dictWithMultipleEntries->tab[i-1], dictWithMultipleEntries->tab[i]) != 0 )
			{
				break;
			}
			else
			{
				free(dictWithMultipleEntries->tab[i]);
			}
			i++;
		}
	}

	printf("Wczytano %d slow!\n", dictionary->size);

	free(dictWithMultipleEntries);

}
