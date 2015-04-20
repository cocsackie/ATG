#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

#include "Debug.h"
#include "TypesAndDefs.h"
#include "DynTab.h"
#include "Util.h"
#include "Generator.h"
#include "IntermediateData.h"
#include "BaseFile.h"
#include "Statistics.h"

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

void InformAboutMissingBaseFileName()
{
	printf("Potrzebny jest przynajmniej jeden plik bazowy!\n");
	exit(EXIT_FAILURE);
}

void InrormAboutMissingOutputFileName()
{
	printf("Podaj nazwę pliku wyjściowego!\n");
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
static DynTab * baseFiles;
static FILE * intermediateFile;
static FILE * statisticsFile;
static FILE * outputFile;
static IntermediateData * intermediateData;

static void closeFile(void * file)
{
	FILE * f = file;
	fclose(f);
}

static void cleanup()
{

	if(intermediateData != NULL)
	{
		IntermediateData_destroy(intermediateData);
	}

	if(statisticsFile != NULL)
	{
		fclose(statisticsFile);
	}
	
	if(intermediateFile != NULL)
	{
		fclose(intermediateFile);
	}
	
	if(baseFiles != NULL)
	{
		DynTab_destroy(baseFiles, closeFile);
	}
	
	if(baseFileNames != NULL)
	{
		DynTab_destroy(baseFileNames, NULL);
	}

	if(outputFile != NULL)
	{
		fclose(outputFile);
	}
}

int main(int argc, char ** argv)
{
	int c, i;
	int index;

	atexit(cleanup);

	baseFileNames = DynTab_create();
	baseFiles = DynTab_create();

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
		InformAboutMissingBaseFileName();
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

	if( outputFileName == NULL )
	{
		InformAboutRequiredArgumentAndExit('o');
	}

	outputFile = fopen(outputFileName, "w+");
	if( outputFile == NULL )
	{
		CantOpenFileError(outputFile);
	}

	for( i = 0; i < baseFileNames->size; i++ )
	{
		FILE * file = fopen(baseFileNames->tab[i], "r");
		
		if( file == NULL )
		{
			CantOpenFileError(baseFileNames->tab[i]);
		}

		DynTab_add(baseFiles, file);
	}

	if( statisticsFileName != NULL )
	{
		statisticsFile = fopen(statisticsFileName, "w");
		
		if(statisticsFile == NULL )
		{
			CantOpenFileError(statisticsFileName);
		}
	}

	if( intermediateFileName != NULL )
	{
		if( baseFileNames->size != 0 )
		{
			intermediateFile = fopen(intermediateFileName, "wb");
		}
		else
		{
			intermediateFile = fopen(intermediateFileName, "rb");
		}

		if(intermediateFile == NULL)
		{
			CantOpenFileError(intermediateFileName);
		}
	}

	if( baseFileNames->size != 0 )
	{	
		intermediateData = BaseFile_loadBaseFilesToIntermediateData( baseFiles, gramTypeValue );
		if( intermediateFileName != NULL )
		{
			IntermediateData_save(intermediateData, intermediateFile);
		}
	}
	else
	{
		intermediateData = IntermediateData_load(intermediateFile);
	}

	Generator_generate(intermediateData, outputFile, wordsValue, paragraphsValue);
	
	if( statisticsFile != NULL )
	{
		Statistics_generateStatistics(intermediateData, statisticsFile, outputFile);
	}


	return EXIT_SUCCESS;	
}
