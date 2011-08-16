/*
 *  queue.h
 *  so-2011
 *
 *  Created by Agustin Marseillan on 8/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "../includes.h"

#ifndef _QUEUE_H_ 
#define _QUEUE_H_


typedef struct queue * queue;

queue	queue_init();

void	queue_poll(queue q, void_p e);

void_p	queue_pull(queue q);

void_p	queue_peek(queue q);

int		queue_empty(queue q);

void	queue_free(queue q);

#endif