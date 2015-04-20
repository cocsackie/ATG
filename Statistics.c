#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "Statistics.h"
#include "DynTab.h"
#include "IntermediateData.h"
#include "Util.h"

const static int topListSize = 10;

static int compareWordOccurences(const void * a, const void * b)
{
	DictionaryEntry * dea = *(DictionaryEntry**) a;
	DictionaryEntry * deb = *(DictionaryEntry**) b;

	if( dea->occurences == deb->occurences)
	{
		return strcmp(dea->word, deb->word);
	}
	
	return dea->occurences < deb->occurences ? 1 : -1;
}

struct Gram
{
	int * prefix;
	int word;
	int occurences;	
};

typedef struct Gram Gram;

static DynTab * grams;
static int allGramsOccurences;
static int allWordOccurences;

static void traverseGrams(void * value)
{
	GramTreeEntry * entry = value;
	int i;

	for( i = 0; i < entry->suffixes->size; i++ )
	{
		Gram * gram = malloc( sizeof(Gram) );
		GramSuffix * suffix = entry->suffixes->tab[i];
		if( gram == NULL )
		{
			OutOfMemoryError();
		}

	
		gram->prefix = entry->prefix;
		gram->word = suffix->word;
		gram->occurences = suffix->occurences;
		
		DynTab_add(grams, gram);
		allGramsOccurences += suffix->occurences;
	}
}

static void createWordStatistics(IntermediateData * data, FILE * file)
{
	int i;
	int topWordsCount = 0;
	int lastOccurences = -1;
	qsort( data->dictionary->tab, data->dictionary->size, sizeof(void*),  compareWordOccurences);

	fprintf(file, "Liczba niepowtarzalnych słów: %d\n", (int)data->dictionary->size);
	fprintf(file, "Najczęściej występujące wyrazy:\n");

	for( i = 0; i < data->dictionary->size; i++ )
	{
		allWordOccurences += ((DictionaryEntry*)data->dictionary->tab[i])->occurences;
	}

	for( i = 0; i < data->dictionary->size && topWordsCount < topListSize; i++ )
	{
		DictionaryEntry * entry = data->dictionary->tab[i];
		double chance = (double)entry->occurences / (double)allWordOccurences*100;
		fprintf(file, "%s : %d (%.2lf%%)\n", entry->word, entry->occurences, chance);

		if( lastOccurences != entry->occurences )
		{
			lastOccurences = entry->occurences;
			topWordsCount++;
		}
	}
}

static int compareGramsOccurences(const void * a, const void * b)
{
	Gram * ga = *(Gram**) a;
	Gram * gb = *(Gram**) b;

	if( ga->occurences == gb->occurences )
	{
		return 0;	
	}

	return ga->occurences < gb->occurences ? 1 : -1;
}

static void createInputStatistics(IntermediateData * data, FILE * file)
{
	int i,j;
	int topGramsCount = 0;
	int lastOccurences = -1;
	createWordStatistics(data, file);
	grams = DynTab_create();
	Tree_traverse(data->gramTree, traverseGrams);
	
	qsort(grams->tab, grams->size, sizeof(void*), compareGramsOccurences);

	fprintf(file, "Najczęściej występujące gramy oraz ich PMI:\n");
	
	for( i = 0; i < grams->size && topGramsCount < topListSize; i++ )
	{
		Gram * gram = grams->tab[i];
		DictionaryEntry * dictSufEntry = data->dictionary->tab[gram->word];
		double wordsProbablility = (double)dictSufEntry->occurences / allWordOccurences;
		double gramProbability = (double)gram->occurences / allGramsOccurences;
		double pmi;
		for( j = 0; j < data->gramType-1; j++ )
		{
			DictionaryEntry * dictEntry = data->dictionary->tab[gram->prefix[j]];
			wordsProbablility *= (double)dictEntry->occurences / allGramsOccurences;
			fprintf(file, "%s ", dictEntry->word);	
		} 

		pmi = gramProbability / wordsProbablility;
		pmi = log(pmi);

		fprintf(file, "%s %.2f (%.4f%%)\n", ((DictionaryEntry*)data->dictionary->tab[gram->word])->word, pmi, gramProbability*100);
	
		if( lastOccurences != gram->occurences )
		{
			lastOccurences = gram->occurences;
			topGramsCount++;
		}
	}

	DynTab_destroy(grams, free);
}
static DynTab * words;
static char word[2048];

static void wordsTreeTraverseHandler(void * value)
{
	char * str = (char *) value;

	if( words->size == 0 )
	{
		DictionaryEntry * entry = DictionaryEntry_create(copyString(str), 1);
		DynTab_add(words, entry);
	}
	else
	{
		if( strcmp(((DictionaryEntry*)words->tab[words->size-1])->word, str) )
		{
			DictionaryEntry * entry = DictionaryEntry_create(copyString(str), 1);
	                DynTab_add(words, entry);
		}
		else
		{
			((DictionaryEntry*)words->tab[words->size-1])->occurences++;
		}
	}
}

static void readWords( FILE * file )
{
	int i,r;
	Tree * tree = Tree_create( (TreeComparator) strcmp );
	words = DynTab_create();

	while( (r = fscanf(file, "%s", word)) != EOF )
	{
		Tree_insert(tree, copyString(word));	
	}

	Tree_traverse( tree, (TreeTraverseHandler) wordsTreeTraverseHandler );
	Tree_destroy( tree, (TreeNodeDestructor) free );
}

void freeWordEntry(void * value)
{
	DictionaryEntry * entry = value;

	free(entry->word);
	free(entry);
}

static void generateOutputStatistics(FILE * statisticsFile, FILE * outputFile)
{
	readWords(outputFile);
	int i;
	int topWordsCount = 0;
	int lastOccurences = -1;
	qsort( words->tab, words->size, sizeof(void*),  compareWordOccurences);

	fprintf(statisticsFile, "Liczba niepowtarzalnych słów: %d\n", (int)words->size);
	fprintf(statisticsFile, "Najczęściej występujące wyrazy:\n");
	
	allWordOccurences = 0;
	for( i = 0; i < words->size; i++ )
	{
		allWordOccurences += ((DictionaryEntry*)words->tab[i])->occurences;
	}

	for( i = 0; i < words->size && topWordsCount < topListSize; i++ )
	{
		DictionaryEntry * entry = words->tab[i];
		double chance = (double)entry->occurences / (double)allWordOccurences*100;
		fprintf(statisticsFile, "%s : %d (%.2lf%%)\n", entry->word, entry->occurences, chance);

		if( lastOccurences != entry->occurences )
		{
			lastOccurences = entry->occurences;
			topWordsCount++;
		}
	}
	DynTab_destroy(words, freeWordEntry);
}

void Statistics_generateStatistics(IntermediateData * data, FILE * statisticsFile, FILE * outputFile)
{
	rewind(outputFile);
	fprintf(statisticsFile, "Dane statystyczne ATG\nDane wejściowe:\n");
	createInputStatistics(data, statisticsFile);

	fprintf(statisticsFile, "\nTekst wygenerowany:\n");
	generateOutputStatistics(statisticsFile, outputFile);
}
