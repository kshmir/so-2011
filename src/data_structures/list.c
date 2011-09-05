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
#include <pthread.h>

typedef struct node * node;

// List node
struct node {
	void_p 			data;
	node	next;
};

// Base list structure
struct list {
	node    header;
	int		size;
	pthread_mutex_t	*	mutex;
};


// Inits the list
list list_init() {
	list ret = (list) malloc(sizeof(struct list));
	ret->size = 0;
	ret->header = NULL;
	ret->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(ret->mutex, NULL);
	return ret;
}

// Adds the pointer to the list.
int list_add(list p, void_p obj) {
	if (p == NULL) {
		return -1;
	}
	
	pthread_mutex_lock(p->mutex);
	node last = NULL;
	node current = p->header;
	node n = NULL;
	
	n = (node) malloc(sizeof(struct node));


	if (n != NULL) {
		n->data = obj;
		n->next = NULL;
	}
	else {
		pthread_mutex_unlock(p->mutex);
		return -1;
	}

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
	pthread_mutex_unlock(p->mutex);
	return 1;
}

// Gets the pointer from a given index
void_p  list_get(list p, int index) {
	if (p == NULL) {
		return NULL;
	}
	pthread_mutex_lock(p->mutex);
	node current = p->header;
	int i = 0;

	while(current) {
		if (i == index) {
			pthread_mutex_unlock(p->mutex);
			return current->data;
		}
		current = current->next;
		i++;
	}
	pthread_mutex_unlock(p->mutex);
	return NULL;
}

// Free's up the list
void list_free_with_data(list p) {
	pthread_mutex_lock(p->mutex);
	pthread_mutex_t * mutex = p->mutex;
	foreach(void_p, n, p) {
		free(n);
	}
	pthread_mutex_unlock(mutex);
	list_free(p);
}

void list_free(list p) {
	if (p == NULL) {
		return;
	}
	pthread_mutex_lock(p->mutex);
	pthread_mutex_t * mutex = p->mutex;
	node actual = p->header;
	free(p);
	while (actual != NULL) {
		node aux = actual->next;
		free(actual);
		actual = aux;
	}
	pthread_mutex_unlock(mutex);
	pthread_mutex_destroy(mutex);
	free(mutex);
}

int list_size(list l) {
	pthread_mutex_lock(l->mutex);
	int size = l->size;
	pthread_mutex_unlock(l->mutex);
	return size;
}



list list_from_ptrarray_w_count(int size, size_t block_size, void_p ptr) {
	list l = list_init();
	int i = 0;
	for(; i < size; i++) {
		list_add(l, ((char*)ptr + block_size * i));
	}
	return l;
}

list list_from_ptrarray_null_ended(char ** values)  {
	list l = list_init();
	int i = 0;
	for(; values[i] != NULL; i++) {
		list_add(l, values[i]);
	}
	return l;
}


// Inserts data to the list.
// Returns -1 if it's a in invalid operation, or the index if it's valid.
int list_insert(list p, int index, void_p  ptr) {
	if (p == NULL || ptr == NULL || index < 0 || index > p->size) {
		return -1;
	}
	pthread_mutex_lock(p->mutex);
	if (index == 0) {
		node old_ref = p->header;
		node ptr_node = (node) malloc(sizeof(struct node));
		ptr_node->data = ptr;
		ptr_node->next = old_ref;
		p->header = ptr_node;
		p->size++;

		pthread_mutex_unlock(p->mutex);
		return index;
	}
	else if (index == p->size) {
		pthread_mutex_unlock(p->mutex);
		list_add(p, ptr);
		return index;
	}
	else {
		node aux = NULL;
		node ptr_node = (node) malloc(sizeof(struct node));
		node cursor = p->header;
	
		int i = 0;
		while(cursor->next && i != index) {
			cursor = cursor->next;
			i++;
		}
		aux = cursor->next;
		ptr_node->next = aux;
		cursor->next = ptr_node;
		
		ptr_node->data = ptr;
		p->size++;
		pthread_mutex_unlock(p->mutex);
		return index;
	}
	return -1;
}

int list_remove(list l, int index) {
	if (index < 0 || l == NULL || index >= l->size) {
		return -1;
	}
	pthread_mutex_lock(l->mutex);
	if (index == 0)
	{
		node n = l->header;
		l->header = n->next;
		free(n);
		l->size--;
	}
	else {
		int i = 0;
		node last = NULL;
		node n = l->header;
		while(i != index) {
			last = n;
			n = n->next;
			i++;
		}
		last->next = n->next;
		free(n);
		l->size--;
	}
	pthread_mutex_unlock(l->mutex);
	return index;
}

int list_indexOf(list p, void_p ptr, comparer comp) {
	if (ptr == NULL)
		return -1;
	pthread_mutex_lock(p->mutex);
	node n = p->header;
	
	int i = 0;
	while(n != NULL)
	{
		if (comp(n->data,ptr) == 0) {
			pthread_mutex_unlock(p->mutex);
			return i;
		}
		
		i++;
		n = n->next;
	}
	pthread_mutex_unlock(p->mutex);
	return -1;
}

void_p list_header(list p) {
	return p->header;
}

void_p list_node_next(void_p n) {
	if (n == NULL) {
		return NULL;
	}
	return ((node)n)->next;
}

void_p list_node_value(void_p n) { 
	if (n == NULL) {
		return NULL;
	}
	
	return ((node)n)->data;
}
