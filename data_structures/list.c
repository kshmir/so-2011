/*
 *  list.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 03/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include "list.h"

#include <stdlib.h>

struct node {
	void *			data;
	struct node *	next;
};


list * init(int (*comparator)(void*, void*)) {
	list * ret = (list *) malloc(sizeof(list));
	ret->size = 0;
	ret->header = NULL;
	ret->comp = comparator;
	return ret;
}

int list_add(list * p, void * obj) {
	
	node * last = NULL;
	node * current = p->header;
	
	node * n = (node *) malloc(sizeof(node));
	n->data = obj;
	
	if (!current) {
		p->header = n;
	}
	else {
		while (current->next) {
			last = current;
			current = current->next;
		}
		last->next = n;
	}
	p->size++;
	
	return 1;
}

void * list_get(list * p, int index) {
	node * current = p->header;
	int i = 0;
	while(current) {
		if (i == index) {
			return current->object;
		}
		current = current->next;
	}
	return NULL;
}