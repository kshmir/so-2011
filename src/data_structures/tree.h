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

#define NegInfinity (-10000)

struct tree;
typedef struct tree * tree;

RedBlackTree MakeEmpty(tree T);
Position Find(ElementType X, RedBlackTree T);
Position FindMin(RedBlackTree T);
Position FindMax(RedBlackTree T);
RedBlackTree Initialize(comparator );
RedBlackTree Insert(ElementType X, RedBlackTree T);
RedBlackTree Remove(ElementType X, RedBlackTree T);
ElementType Retrieve(Position P);
void PrintTree(RedBlackTree T);











#endif 
