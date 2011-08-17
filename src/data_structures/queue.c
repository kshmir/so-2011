/*
 *  queue.c
 *  so-2011
 *
 *  Created by Agustin Marseillan on 8/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "queue.h"
#include "list.h"

struct queue{
	list l;
};

queue	queue_init(){
	queue q = malloc(sizeof(struct queue));
	q->l = list_init();
	return q;
}

void	queue_poll(queue q, void_p e){
	list_add(q->l, e);
}

void_p	queue_pull(queue q){
	void_p ret = list_get(q->l, 0);
	list_remove(q->l, 0);
	
	return ret;
}

void_p	queue_peek(queue q){
	void_p ret = list_get(q->l, 0);
	return ret;
}

int		queue_empty(queue q) {
	
	return list_size(q->l) == 0;
}

void	queue_free(queue q){
	list_free(q->l);
	free(q);
}