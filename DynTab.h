#ifndef __DYNTABLE_H__
#define __DYNTABLE_H__

#include "TypesAndDefs.h"

struct DynTab
{
	size_t size;
	size_t capacity;
	void ** tab;
};

typedef struct DynTab DynTab;

DynTab * DynTab_create();
boolean DynTab_add(DynTab * tab, void * element);

int DynTab_binsearch(DynTab * tab, void * element, int (*comp)(const void *, const void*));

#endif
