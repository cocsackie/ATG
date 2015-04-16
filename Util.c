#include<stdlib.h>
#include<errno.h>
#include"Util.h"
#include"Error.h"
#include"string.h"

char * copyString(const char * str)
{
	char * copyStr = malloc( strlen( str ) + 1 );

	if( copyStr == NULL )
	{
		OutOfMemoryError();
	}

	strcpy( copyStr, str);
	return copyStr;
}

// returns 1 if non-number 

int parseInt(const char * str, int * value)
{
	errno = 0;
	*value = strtol(str, NULL, 10);

	return errno != 0;
} 
