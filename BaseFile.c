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
static IntermediateData * data;

struct GramTreeEntry
{
	Tree * tree;
	int word;
};

typedef struct GramTreeEntry GramTreeEntry;

struct GramTreeLeaf
{
	GramTreeEntry entry;
	int occurence;
};

typedef struct GramTreeLeaf GramTreeLeaf;

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

static int gtecmp( const void * a, const void * b )
{
	GramTreeEntry * pa = (GramTreeEntry*) a;
	GramTreeEntry * pb = (GramTreeEntry*) b;
	
	if(pa->word == pb->word)
	{
		return 0;
	}

	return pa->word < pb->word ? -1 : 1;
}

static boolean createGramTreeFromFileAndDictionary(const char * fileName, int gramType)
{
	Tree * currTree;
	DynTab * lastWords = DynTab_create();
	FILE * file = fopen(fileName, "r");
	int r;
	int i;

	while( (r = fscanf(file, "%s", word)) != EOF )
	{
		int wordIndex = DynTab_binsearch(dictionary, word, dictionaryWordCompare);

		if(wordIndex == -1)
		{
			//TODO: fail
			fclose(file);
			return FALSE;
		}

		if( lastWords->size < gramType-1 )
		{
			DynTab_add(lastWords, dictionary->tab[wordIndex]);
		}
		else
		{	
			GramTreeEntry * entry;
			GramTreeLeaf * leaf;
			for( i = 0; i < lastWords->size-1; i-- )
			{
				lastWords->tab[i] = lastWords->tab[i+1];
			}

			currTree = tree;

			for( i = 0; i < gramType-1; i++ )
			{
				entry = Tree_find(currTree, lastWords->tab[i], gtecmp);
				
				if( entry != NULL )
				{
					currTree = entry->tree;
				}
				else
				{
					entry = malloc( sizeof( GramTreeEntry ) );
					
					if( entry == NULL )
					{
						//TODO: fail
						fclose(file);
						return FALSE;
					}
					
					entry->word = (int) lastWords->tab[i];
					entry->tree = Tree_create( (TreeComparator) gtecmp );
					Tree_insert(currTree, entry); 
					currTree = entry->tree;
				}
			}

			leaf = Tree_find(currTree, lastWords->tab[gramType-1], gtecmp);
			if( leaf != NULL )
			{
				leaf->occurence++;
			}			
			else
			{
				leaf = malloc( sizeof( GramTreeLeaf ) );
				
				if( leaf == NULL )
				{
					//TODO: fail
				}

				leaf->entry.word = (int) lastWords->tab[gramType-1];
				leaf->entry.tree = NULL;
				Tree_insert(currTree, leaf);
			}
		}
	}
	
	fclose(file);
	return TRUE;
}

static int gType;
static DynTab * stackTree;
static DynTab * stackTable;
static DynTab * handlerReturnTable;
static int depth = 0;

static void treeToTabTraverseHandler(void * value)
{
	if( depth != gType-1 )
	{
		GramTreeEntry * entry = (GramTreeEntry*) value;
	
		GramTrieEntry * trieEntry = malloc( sizeof( GramTrieEntry ) );

		if( trieEntry == NULL )
		{
			exit(0);
		}

		trieEntry->word = entry->word;
		trieEntry->suffixes = DynTab_create();

		if( trieEntry->suffixes == NULL )
		{
			exit(0);
		}

		DynTab_add( stackTable->tab[stackTable->size-1], trieEntry);
	
		depth++;
		DynTab_add( stackTree, entry->tree );
		DynTab_add( stackTable, trieEntry->suffixes );
		Tree_traverse( stackTree->tab[stackTree->size-1], (TreeTraverseHandler) treeToTabTraverseHandler );
		stackTree->size--;
		stackTable->size--;
		depth--;
	} else {
		GramTreeLeaf * leaf = (GramTreeLeaf*) value;
		GramTrieLeaf * trieLeaf = malloc( sizeof( GramTrieLeaf ) );

		if( trieLeaf == NULL )
		{
			exit(0);
		}
	
		trieLeaf->word = leaf->entry.word;
		trieLeaf->occurence = leaf->occurence;

		DynTab_add(stackTable->tab[stackTable->size-1], trieLeaf);

		return;
	}
}

static boolean createGramTreeFromFilesAndDictionary(DynTab * fileNames, int gramType)
{
	int i;
	Tree * currTree;
	GramTrieRoot * gramTrieRoot;
	tree = Tree_create( (TreeComparator) gtecmp );

	for(i = 0; i < fileNames->size; i++)
	{
		boolean success = createGramTreeFromFileAndDictionary(fileNames->tab[i], gramType);

		if( success == FALSE )
		{
			//TODO: destroy tree
			return FALSE;
		}
	}

	
	
	data = malloc( sizeof( IntermediateData ) );
	gramTrieRoot = malloc( sizeof( GramTrieRoot ) );

	if( data == NULL || gramTrieRoot == NULL )
	{
		//TODO: exit
		return FALSE;
	}
	
	currTree = tree;

	stackTree = DynTab_create();
	stackTable = DynTab_create();
	handlerReturnTable = DynTab_create();
	gType = gramType;
	DynTab_add(stackTable, handlerReturnTable);

	for( i = 0; i < gramType-1; i++ )
	{	
		if( handlerReturnTable == NULL )
		{
			return FALSE;
			//exit();
		}

		Tree_traverse(currTree, (TreeTraverseHandler) treeToTabTraverseHandler);
	}

	data->dictionary = dictionary;
	data->gramTrieRoot = gramTrieRoot;
	data->gramType = gramType;

	gramTrieRoot->suffixes = handlerReturnTable;
	
	//TODO:destroy tree

	return TRUE;
}

IntermediateData * BaseFile_loadBaseFilesToIntermediateData(DynTab * fileNames, int gramType)
{
	boolean success = createDictionaryFromFiles(fileNames);
	createGramTreeFromFilesAndDictionary(fileNames, gramType);
	return data;	
}
