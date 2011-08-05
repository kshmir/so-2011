/*
 *  stack.h
 *  so-2011
 *
 *  Created by Cristian Pereyra on 05/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include <stdlib.h>



typedef struct stack stack;

stack * stack_init(size_t start_size);

int stack_count(stack * pila);

int stack_is_empty(stack * pila);

int stack_push(stack * pila, void * object);

void * stack_pop(stack * pila);

stack * stack_free(stack * pila);
