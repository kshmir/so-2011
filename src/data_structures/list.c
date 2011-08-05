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
#include <stdio.h>

struct node {
	void *			data;
	struct node *	next;
};


list * list_init(int (*comparator)(void*, void*)) {
	list * ret = (list *) malloc(sizeof(list));
	ret->size = 0;
	ret->header = NULL;
	ret->comparator = comparator;
	return ret;
}

int list_add(list * p, void * obj) {

	node * last = NULL;
	node * current = p->header;
	
	
	node * n = (node *) malloc(sizeof(node));
	n->data = obj;


	if (current == NULL) {
		p->header = n;
	}
	else {
		while (current) {
			last = current;
			current = current->next;
		}
		if (last) {
			last->next = n;
		}
		else {
			p->header->next = n;
		}

	}
	p->size++;
	
	return 1;
}

void * list_get(list * p, int index) {
	node * current = p->header;
	int i = 0;

	while(current) {
		if (i == index) {
			return current->data;
		}
		current = current->next;
		i++;
	}

	return NULL;
}

void list_free(list * p) {
	if (p == NULL) {
		return;
	}
	node * actual = p->header;
	free(p);
	while (actual != NULL) {
		node * aux = actual->next;
		free(actual);
	}
}