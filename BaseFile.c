#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "TypesAndDefs.h"
#include "BaseFile.h"
#include "IntermediateData.h"
#include "Tree.h"

static Tree * tree;
static DynTab * dictionary;
static char word[2048];

static char * copyString(const char * str)
{
	char * copyStr = malloc( strlen( str ) + 1 );
	strcpy( copyStr, str );

	return copyStr;
}

static boolean addAllWordsFromFileToTree(const char * fileName)
{	
	FILE * file = fopen(fileName, "r");
	int r;

	while( (r = fscanf(file, "%s", word)) != EOF )
	{
		boolean success = Tree_insert(tree, copyString(word));

		if( success == FALSE )
		{
			fclose(file);
			return FALSE;
		}
	}
	
	fclose(file);
	return TRUE;
}

static void traverseHandler(void * value)
{
	char * str = (char *) value;
	
	if( dictionary->size == 0 )
	{
		DictionaryEntry * dictEntry = DictionaryEntry_create(copyString(str), 1);
		DynTab_add(dictionary, dictEntry);
	}
	else
	{
		if( strcmp(((DictionaryEntry*)dictionary->tab[dictionary->size-1])->word, str) )
		{
			DictionaryEntry * dictEntry = DictionaryEntry_create(copyString(str), 1);
			DynTab_add(dictionary, dictEntry);
		}
		else
		{
			((DictionaryEntry*)dictionary->tab[dictionary->size-1])->occurences++;
		}
	}
}

static boolean createDictionaryFromFiles(DynTab * fileNames)
{
	int i;
	tree = Tree_create( (TreeComparator) strcmp);
	dictionary = DynTab_create();

	for(i = 0; i < fileNames->size; i++)
	{
		boolean success = addAllWordsFromFileToTree(fileNames->tab[i]);
		
		if( success == FALSE )
		{
			Tree_destroy( tree, (TreeNodeDestructor) free );
			return FALSE;
		}
	}

	Tree_traverse( tree, (TreeTraverseHandler) traverseHandler );
	Tree_destroy( tree, (TreeNodeDestructor) free );
	return TRUE;
}

IntermediateData * BaseFile_loadBaseFilesToIntermediateData(DynTab * fileNames, int gramType)
{
	boolean success = createDictionaryFromFiles(fileNames);
	createGramTreeFromFilesAndDictionary(fileNames, gramType);	
}
