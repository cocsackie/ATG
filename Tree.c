#include <stdlib.h>

#include "Tree.h"
#include "TypesAndDefs.h"

Tree * Tree_create(TreeComparator cmp)
{
	Tree * tree = malloc( sizeof(Tree) );

	if( tree != NULL )
	{
		tree->cmp = cmp;	
	}

	return tree;	
}

static void destroySubTree(Node * node, TreeNodeDestructor destructor)
{
	if(node == NULL)
	{
		return;
	}
	
	destroySubTree(node->left, destructor);
	destroySubTree(node->right, destructor);

	if(destructor != NULL)
	{
		destructor(node->value);
	}

	free(node);
}

void Tree_destroy(Tree * tree, TreeNodeDestructor destructor)
{
	destroySubTree(tree->root, destructor);
	free(tree);
}

static Node * createNode()
{
	Node * node = malloc( sizeof(Node) );
	
	if( node != NULL )
	{
		node->left = NULL;
		node->right = NULL;
		node->value = NULL;
	}
	
	return node;
}

boolean Tree_insert(Tree * tree, void * value)
{
	Node * prev = NULL;
	Node * current = tree->root;
	Node * new = NULL;
	int result;
	new = createNode();	
	if( new	== NULL )
	{
		return FALSE;
	}	
	new->value = value;
	
	while( current != NULL )
	{
		result = tree->cmp(value, current->value);
		prev = current;
		if( result < 0 )
		{
			current = current->left;
		}
		else
		{
			current = current->right;
		}
	}

	if(prev == NULL)
	{
		tree->root = new;
	}
	else
	{
		if( result < 0 )
		{
			prev->left = new;
		}
		else
		{
			prev->right = new;
		}
	}

	return TRUE;
}

static void traverse(Node * node, TreeTraverseHandler handler)
{
	if( node == NULL )
	{
		return;
	}
	
	traverse(node->left, handler);

	if(handler != NULL)
	{
		handler(node->value);
	}

	traverse(node->right, handler);
}

void Tree_traverse(Tree * tree, TreeTraverseHandler handler)
{
	traverse(tree->root, handler);	
}
