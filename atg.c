#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

#include "Debug.h"
#include "TypesAndDefs.h"
#include "DynTab.h"
#include "Util.h"
#include "Generator.h"

void InformAboutRequiredArgumentAndExit()
{
	printf("Nie podałeś argumentu dla flagi -%c!\n", optopt);
	exit(EXIT_FAILURE);
}

void InformAboutInvalidArgumentAndExit(int c)
{
	printf("Podałeś zły argument dla flagi -%c!\n", c);
	exit(EXIT_FAILURE);
}

void InformAboutUnknownFlagAndExit()
{
	printf("Nieznana flaga -%c!\n", optopt);
	exit(EXIT_FAILURE);
}

void HandleRepeatedFlagAndExit(int c)
{
	printf("Powtórzyłeś flagę -%c!\n", c);
	exit(EXIT_FAILURE);
}

void HandleGetOptErrorAndExit()
{
	switch(optopt)
	{
		case 'b':
		case 'i':
		case 'o':
		case 's':
		case 'w':
		case 'p':
		case 'n':
		{
			InformAboutRequiredArgumentAndExit();
		}
		default:
		{
			InformAboutUnknownFlagAndExit();
		}
	}
}

static DynTab * baseFileNames;

static void cleanup()
{
	if(baseFileNames != NULL)
	{
		free(baseFileNames);
	}
}

int main(int argc, char ** argv)
{
	int c;
	int index;

	baseFileNames = DynTab_create();

	boolean intermediateFlag = FALSE;
	char * intermediateFileName = NULL;

	boolean outputFlag = FALSE;
	char * outputFileName = NULL;

	boolean statisticsFlag = FALSE;
	char * statisticsFileName = NULL;

	boolean wordsFlag = FALSE;
	char * words = NULL;
	int wordsValue = 100;

	boolean paragraphsFlag = FALSE;
	char * paragraphs = NULL;
	int paragraphsValue = 1;

	boolean gramTypeFlag = FALSE;
	char * gramType = NULL;
	int gramTypeValue = 2;
	IntermediateData * intermediateData;

	opterr = 0; //disable optget error messages

	while((c = getopt(argc, argv, "b:i:o:s:w:p:n:")) != -1)
	{
		switch(c)
		{
			case 'b':
			{
				index = optind-1;
				while(index < argc)
				{
					DynTab_add(baseFileNames,argv[index]);
					index++;
					if(index < argc && argv[index][0] == '-')
					{
						break;
					}
				}
				optind = index;
				break;
			}
			case 'i':
			{
				if( intermediateFlag != FALSE )
				{
					HandleRepeatedFlagAndExit(c);
				}
				
				intermediateFileName = optarg;
				intermediateFlag = TRUE;	
				break;
			}
			case 'o':
			{
				if( outputFlag != FALSE )
				{
					HandleRepeatedFlagAndExit(c);
				}

				outputFileName = optarg;
				outputFlag = TRUE;
				break;
			}
			case 's':
			{
				if( statisticsFlag != FALSE )
				{
					HandleRepeatedFlagAndExit(c);
				}

				statisticsFileName = optarg;
				statisticsFlag = TRUE;
				break;
			}
			case 'w':
			{
				if( wordsFlag != FALSE )
				{
					HandleRepeatedFlagAndExit(c);
				}

				words = optarg;
				wordsFlag = TRUE;
				break;
			}
			case 'p':
			{
				if( paragraphsFlag != FALSE )
				{
					HandleRepeatedFlagAndExit(c);
				}

				paragraphs = optarg;
				paragraphsFlag = TRUE;
				break;
			}
			case 'n':
			{
				if( gramTypeFlag != FALSE )
				{
					HandleGetOptErrorAndExit(c);
				}

				gramType = optarg;
				gramTypeFlag = TRUE;
				break;
			}
			case '?':
			{
				HandleGetOptErrorAndExit();
			}
			default:
			{
				exit(EXIT_FAILURE);
			}
		}
	}

	if(optind < argc)
	{
		HandleGetOptErrorAndExit();
	}

	if(wordsFlag && parseInt(words, &wordsValue))
	{
		InformAboutInvalidArgumentAndExit('w');
	}

	if(paragraphsFlag && parseInt(paragraphs, &paragraphsValue))
	{
		InformAboutInvalidArgumentAndExit('p');
	}

	if(gramTypeFlag && parseInt(gramType, &gramTypeValue))
	{
		InformAboutInvalidArgumentAndExit('p');
	}

	if( baseFileNames->size == 0 && intermediateFileName == NULL )
	{
		//TODO: missing basefile name/s
	}

	if( gramTypeValue <= 0 )
	{
		InformAboutInvalidArgumentAndExit('n');
	}

	if( wordsValue <= 0 || wordsValue < gramTypeValue )
	{
		InformAboutInvalidArgumentAndExit('w');
	}

	if( paragraphsValue <= 0 )
	{
		InformAboutInvalidArgumentAndExit('p');
	}

	if( baseFileNames->size != 0 )
	{	
		intermediateData = BaseFile_loadBaseFilesToIntermediateData( baseFileNames, gramTypeValue );
		if( intermediateFileName != NULL )
		{
			//TODO: save
		}
	}
	else
	{
		//intermediateData = NULL; //TODO: read
	}

	//TODO: statistics

	Generator_generate(intermediateData, stdout, wordsValue, paragraphsValue);

	
	
	#ifndef NDEBUG
	printf("Pliki bazowe:\n");
	{
		int i;
		for( i = 0; i < baseFileNames->size; i++ )
		{
			printf("- %s\n", (char *)baseFileNames->tab[i]);	
		}	 
	}

	printf("intermediateFileName: %s\n", intermediateFileName);
	printf("outputFileName: %s\n", outputFileName);
	printf("statisticsFileName: %s\n", statisticsFileName);
	printf("words: %d\n", wordsValue);
	printf("paragraphs: %d\n", paragraphsValue);
	printf("gramType: %d\n", gramTypeValue);	
	
	#endif

	return EXIT_SUCCESS;	
}
