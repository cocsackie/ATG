#ifndef __INTERMEDIATE_DATA_H__
#define __INTERMEDIATE_DATA_H__

#include <stdio.h>

#include "DynTab.h"
#include "Tree.h"

struct IntermediateData
{
	DynTab * dictionary;
	Tree * gramTree;
	int gramType;
};

typedef struct IntermediateData IntermediateData;

struct DictionaryEntry
{
	char * word;
	int occurences;
};

typedef struct DictionaryEntry DictionaryEntry;

struct GramTreeEntry
{
	int * prefix;
	int prefixSize;
	DynTab * suffixes;
};

typedef struct GramTreeEntry GramTreeEntry;

struct GramSuffix
{
	int word;
	int occurences;
};

typedef struct GramSuffix GramSuffix;

IntermediateData * IntermediateData_create();
DictionaryEntry * DictionaryEntry_create(char * word, int occurences);

int dictionaryWordCompare(const void * a, const void * b);
GramTreeEntry * GramTreeEntry_create(int * prefix, int gramSize);
GramSuffix * GramSuffix_create();
void IntermediateData_save(IntermediateData * data, FILE * file);
IntermediateData * IntermediateData_load(FILE * file);
#endif
