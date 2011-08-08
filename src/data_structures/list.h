/*
 *  list.h
 *  so-2011
 *
 *  Created by Cristian Pereyra on 03/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include "../includes.h"

#ifndef _LIST_H_
#define _LIST_H_

// This makes all calls to list equal to "struct list *"
// So BEWARE that any call to list is a pointer by itself.
typedef struct list * list;

// Inits the list
list list_init();

// Free's up the list
void list_free();

// Adds an element to the list.
int list_add(list p, void_p obj);

// Size of the given list
int list_size(list p);

// Retreives an element in the given index of the list
void_p list_get(list p, int index);

// Retreives an element in the given index of the list
void_p list_get(list p, int index);

// Inserts an element in the given index of the list
int list_indexOf(list p, void_p ptr, comparer comp);

// Removes an element in the given index of the list
int list_remove(list p, int index);

#endif
