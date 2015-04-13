#ifndef __INTERMEDIATE_DATA_H__
#define __INTERMEDIATE_DATA_H__

#include "DynTab.h"

struct GramTrieRoot
{
	DynTab * suffixes;
};

typedef struct GramTrieRoot GramTrieRoot;

struct GramTrieEntry
{
	DynTab * suffixes;
	int word;
};

typedef struct GramTrieEntry GramTrieEntry;

struct GramTrieLeaf
{
	int word;
	int occurence;
};

typedef struct GramTrieLeaf GramTrieLeaf;

struct IntermediateData
{
	DynTab * dictionary;
	GramTrieRoot * gramTrieRoot;
	int gramType;
};

typedef struct IntermediateData IntermediateData;

struct DictionaryEntry
{
	char * word;
	int occurences;
};

typedef struct DictionaryEntry DictionaryEntry;

DictionaryEntry * DictionaryEntry_create(char * word, int occurences);

int dictionaryWordCompare(const void * a, const void * b);

#endif
