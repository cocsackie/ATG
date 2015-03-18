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

static Trie * trie;
static DynTab * dictionary;
static char word[512];

static boolean insertIntoTrie(const char * str)
{
	Trie * t = trie;
	while( *str != '\0' )
	{
		if( t->next[*str] == NULL )
		{
			t->next[*str] = malloc( sizeof( Trie ) );
			if( t->next[*str] == NULL )
			{
				return FALSE;
			}
			t->next[*str]->cnt = 0;
		}
		t = t->next[*str];
		str++;
	}
	t->cnt++;
}

static char * copyString(const char * str)
{
	char * copyStr = malloc( strlen( str ) + 1 );
	strcpy( copyStr, str );
	return copyStr;
}

static void traverseTrie(Trie * trie, int level)
{
	int i;
	word[level] = '\0';
	if( trie->cnt != 0 )
	{
		DynTab_add(dictionary, copyString(word)); 
	}
	for( i = 0; i < 256; i++ )
	{
		if( trie->next[i] != NULL )
		{
			word[level] = (char) i;
			traverseTrie(trie->next[i], level+1);
		}
	}
}

static boolean readFile(const char * fileName)
{
	FILE * file = fopen(fileName, "r");
	int r;

	while( (r = fscanf(file, "%s", word)) != EOF )
	{
		insertIntoTrie(word);
	}	

}

struct IntermediateData * BaseFile_loadBaseFilesToIntermediateData(DynTab * fileNames)
{
	int i;
	dictionary = DynTab_create();
	trie = malloc( sizeof( Trie ) );	
	for( i = 0; i < fileNames->size; i++ )
	{
		readFile( (char *)fileNames->tab[i] );
	} 
	traverseTrie(trie, 0);
}
