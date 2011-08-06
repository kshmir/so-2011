/*
 *  stack.h
 *  so-2011
 *
 *  Created by Cristian Pereyra on 05/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */


#include "../includes.h"
#include <stdlib.h>

#ifndef _STACK_H_ 
#define _STACK_H_

// TODO: Make it shrinkable 

// This makes all calls to stack equal to "struct stack *"
// So BEWARE that any call to stack is a pointer by itself.
typedef struct stack * stack;

// Starts the stack with a given size
stack stack_init(size_t start_size);

// Tells the amount of elements in the stack
int stack_count(stack pila);

// Tells whether the stack is empty or not
int stack_is_empty(stack pila);

// Pushes an element to the stack
int stack_push(stack pila, void_p object);

// Pops an element from the stack
void_p stack_pop(stack pila);


// Free's up the stack
stack stack_free(stack pila);

#endif
