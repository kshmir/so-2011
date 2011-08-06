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

typedef struct list list;

list * list_init();

void list_free();

int list_add(list * p, void_p obj);

int list_size(list * p);

void_p list_get(list * p, int index);

int list_insert(list * p, int index, void_p ptr);

#endif
