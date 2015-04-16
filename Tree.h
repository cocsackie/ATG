#ifndef __TREE_H__
#define __TREE_H__

#include "TypesAndDefs.h"

typedef int (*TreeComparator)(const void *, const void *);
typedef void (*TreeNodeDestructor)(void *);
typedef void (*TreeTraverseHandler)(void *);

struct Node
{
	struct Node * left;
	struct Node * right;
	void * value;
};

typedef struct Node Node;

struct Tree
{
	Node * root;
	int size;
	TreeComparator cmp;
};

typedef struct Tree Tree;



Tree * Tree_create(TreeComparator cmp);
void Tree_destroy(Tree * tree, TreeNodeDestructor destructor);
boolean Tree_insert(Tree * tree, void * value);
void Tree_traverse(Tree * tree, TreeTraverseHandler handler);
void * Tree_find(Tree * tree, void * value, TreeComparator comparator);

#endif
