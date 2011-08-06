/*
 *	WE GOT THE BASIC IMPLEMENTATION OF THE RED-BLACK TREE FROM 
 *  HERE: http://cprogramminglanguage.net/red-black-tree-c-code.aspx
 *  WE MADE IT GENERIC
 *  tree.h
 *  so-2011
 */

#include "../includes.h"
#include <stdlib.h>

#ifndef	_TREE_H_
#define _TREE_H_


struct tree;

typedef struct tree * tree;

void_p tree_delete(tree t, void_p e);

int tree_add(tree t, void_p e);

void tree_print(tree t, printer p);

void_p tree_get(tree t, void_p e);

int tree_size(tree t);

tree tree_init(comparer comp);

void tree_free(tree t);

#endif 
