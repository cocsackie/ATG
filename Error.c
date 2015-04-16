#include<stdio.h>
#include<stdlib.h>

void ShowErrorAndExit(char * error)
{
	printf("%s", error);
	exit(EXIT_FAILURE);
}

void OutOfMemoryError()
{
	printf("Zabrakło pamięci!\n");
	exit(EXIT_FAILURE);
}

void CantOpenFileError(char * fileName)
{
	printf("Nie mogę otworzyć pliku %s!\n", fileName);
	exit(EXIT_FAILURE);
}
