/*
 *  list.h
 *  so-2011
 *
 *  Created by Cristian Pereyra on 03/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */



typedef struct node node;

struct list {
	node *	header;
	int		size;
	int		(*comparator)(void*, void*);
} typedef list;


list * list_init();

void list_free();

int list_add(list * p, void * obj);

void * list_get(list * p, int index);

int list_insert(list * p, int index, void * ptr);