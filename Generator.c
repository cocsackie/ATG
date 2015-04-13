#include<stdlib.h>

#include"Generator.h"
#include"IntermediateData.h"
#include"DynTab.h"

static int binsearchComparator(const void * a, const void *b)
{
}

void Generator_generate(IntermediateData * intermediateData, FILE * file, int words, int paragraphs)
{
	/*
	int gramType = intermediateData->gramType;
	DynTab text = DynTab_create();

	DynTab * suffixes = intermediateData->gramTrieRoot->suffixes;

	srand(time(NULL));

	for(int i = 0; i < gramType-1; i++)
	{
		DynTab_add(text, rand()%(suffixes->size-1));
		DynTab_binsearch(suffixes, 
	}
	*/
}
