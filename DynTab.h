#ifndef __DYNTABLE_H__
#define __DYNTABLE_H__

#include "TypesAndDefs.h"

typedef int (*DynTabComparator)(const void *, const void *);
typedef void (*DynTabValueDestructor)(void *);

struct DynTab
{
	size_t size;
	size_t capacity;
	void ** tab;
};

typedef struct DynTab DynTab;

DynTab * DynTab_create();
void DynTab_destroy(DynTab * tab, DynTabValueDestructor destructor);
void DynTab_resize(DynTab * tab, int size);
void DynTab_add(DynTab * tab, void * element);

int DynTab_binsearch(DynTab * tab, void * element, DynTabComparator comparator);

int DynTab_search(DynTab * tab, void * element, DynTabComparator comparator);

#endif
