/*
 *  tree.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 05/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include "tree.h"
#include <stdlib.h>
#include <stdio.h>


typedef enum ColorType {
    Red, Black
} ColorType;

struct node {
    void_p Element;
    struct node * Left;
    struct node * Right;
    ColorType Color;
} typedef node;

struct tree {
	int size;
	comparer comp;
	node * head;
};

node * NullNode = NULL; /* Needs initialization */

/* Initialization procedure */
tree
tree_init(comparer comp) {
	tree t;
    node * head;
	
    if (NullNode == NULL) {
        NullNode = malloc(sizeof(node));
        if (NullNode == NULL)
            FatalError("Out of space!!!");
        NullNode->Left = NullNode->Right = NullNode;
        NullNode->Color = Black;
        NullNode->Element = 12345;
    }
	
    /* Create the header node */
    head = malloc(sizeof(node));
    if (head == NULL)
        FatalError("Out of space!!!");
    head->Element = NegInfinity;
    head->Left = head->Right = NullNode;
    head->Color = Black;
	
	t = malloc(sizeof(struct tree));
	t->head = head;
	t->size = size;
	t->comparer = comp;
	
    return t;
}

/* END */

void
Output(ElementType Element) {
    printf("%d\n", Element);
}


/* Print the tree, watch out for NullNode, */

/* and skip header */

static void
DoPrint(RedBlackTree T) {
    if (T != NullNode) {
        DoPrint(T->Left);
        Output(T->Element);
        DoPrint(T->Right);
    }
}

void
PrintTree(RedBlackTree T) {
    DoPrint(T->Right);
}

/* END */

static RedBlackTree
MakeEmptyRec(RedBlackTree T) {
    if (T != NullNode) {
        MakeEmptyRec(T->Left);
        MakeEmptyRec(T->Right);
        free(T);
    }
    return NullNode;
}

RedBlackTree
MakeEmpty(RedBlackTree T) {
    T->Right = MakeEmptyRec(T->Right);
    return T;
}

Position
Find(ElementType X, RedBlackTree T) {
    if (T == NullNode)
        return NullNode;
    if (X < T->Element)
        return Find(X, T->Left);
    else
        if (X > T->Element)
			return Find(X, T->Right);
		else
			return T;
}

Position
FindMin(RedBlackTree T) {
    T = T->Right;
    while (T->Left != NullNode)
        T = T->Left;
	
    return T;
}

Position
FindMax(RedBlackTree T) {
    while (T->Right != NullNode)
        T = T->Right;
	
    return T;
}

/* This function can be called only if K2 has a left child */
/* Perform a rotate between a node (K2) and its left child */

/* Update heights, then return new root */

static Position
SingleRotateWithLeft(Position K2) {
    Position K1;
	
    K1 = K2->Left;
    K2->Left = K1->Right;
    K1->Right = K2;
	
    return K1; /* New root */
}

/* This function can be called only if K1 has a right child */
/* Perform a rotate between a node (K1) and its right child */

/* Update heights, then return new root */

static Position
SingleRotateWithRight(Position K1) {
    Position K2;
	
    K2 = K1->Right;
    K1->Right = K2->Left;
    K2->Left = K1;
	
    return K2; /* New root */
}


/* Perform a rotation at node X */
/* (whose parent is passed as a parameter) */

/* The child is deduced by examining Item */

static Position
Rotate(ElementType Item, Position Parent) {
	
    if (Item < Parent->Element)
        return Parent->Left = Item < Parent->Left->Element ?
		SingleRotateWithLeft(Parent->Left) :
        SingleRotateWithRight(Parent->Left);
    else
        return Parent->Right = Item < Parent->Right->Element ?
		SingleRotateWithLeft(Parent->Right) :
        SingleRotateWithRight(Parent->Right);
}



static Position X, P, GP, GGP;

static
void HandleReorient(ElementType Item, RedBlackTree T) {
    X->Color = Red; /* Do the color flip */
    X->Left->Color = Black;
    X->Right->Color = Black;
	
    if (P->Color == Red) /* Have to rotate */ {
        GP->Color = Red;
        if ((Item < GP->Element) != (Item < P->Element))
            P = Rotate(Item, GP); /* Start double rotate */
        X = Rotate(Item, GGP);
        X->Color = Black;
    }
    T->Right->Color = Black; /* Make root black */
}

RedBlackTree
Insert(ElementType Item, RedBlackTree T) {
    X = P = GP = T;
    NullNode->Element = Item;
    while (X->Element != Item) /* Descend down the tree */ {
        GGP = GP;
        GP = P;
        P = X;
        if (Item < X->Element)
            X = X->Left;
        else
            X = X->Right;
        if (X->Left->Color == Red && X->Right->Color == Red)
            HandleReorient(Item, T);
    }
	
    if (X != NullNode)
        return NullNode; /* Duplicate */
	
    X = malloc(sizeof ( struct RedBlackNode));
    if (X == NULL)
        FatalError("Out of space!!!");
    X->Element = Item;
    X->Left = X->Right = NullNode;
	
    if (Item < P->Element) /* Attach to its parent */
        P->Left = X;
    else
        P->Right = X;
    HandleReorient(Item, T); /* Color it red; maybe rotate */
	
    return T;
}

RedBlackTree
Remove(ElementType Item, RedBlackTree T) {
    printf("Remove is unimplemented\n");
    if (Item)
        return T;
    return T;
}

ElementType
Retrieve(Position P) {
    return P->Element;
}


