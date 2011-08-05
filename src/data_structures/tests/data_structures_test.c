/*
 *  test.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 04/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../stack.h"
#include "../list.h"

void separator() {
	printf("------------------------------------\n");	
}

void list_test() {
	separator();
	printf("Testing list repeated elements\n");
	
	list * lista = list_init(NULL);
	
	if (lista == NULL) {
		return;
	}
	
	printf("It should start the list\n");
	assert(lista != NULL);
	printf("DONE!\n");
	
	int * array = malloc(sizeof(int) * 10);
	int i = 0;
	for (; i < 10; i++) {
		array[i] = i;
	}
	
	printf("Adding an element should return 1\n");
	for (i=0; i < 10; i++) {
		assert(list_add(lista, array + i) == 1); 
	}
	printf("DONE!\n");
	
	
	printf("I should get the value with an index from 0 to n-1\n");
	assert(*((int*)list_get(lista, 4)) == 4);		
	assert(*((int*)list_get(lista, 0)) == 0);		
	assert(*((int*)list_get(lista, 9)) == 9);		
	printf("DONE!\n");
	
	printf("I should get null if the index is not between 0 to n-1\n");
	assert(list_get(lista, -1) == NULL);		
	assert(list_get(lista, 11) == NULL);		
	printf("DONE!\n");
	
	printf("I should be able to insert between 0 to n\n");
	int * pointer = &i;
	assert(list_insert(lista, 11, pointer) == -1);		
	assert(list_insert(lista, 10, pointer) == 10);		
	assert(list_insert(lista, -5, pointer) == -1);
	assert(list_insert(lista, -1, pointer) == -1);		
	assert(list_insert(lista, 4, pointer) == 4);
	assert(list_insert(lista, 0, pointer) == 0);
	assert(list_insert(lista, 1, pointer) == 1);
	printf("DONE!\n");
	list_free(lista);
	free(array);
	separator();
	return;
}

void stack_test() {
	
	separator();
	printf("Testing stack repeated elements\n");
	stack * pila = stack_init(1);
	int arreglo[] = {1,2,3,4,5,6,7,8,9,10};

	printf("It should push data to the stack\n");
	assert(stack_push(pila, arreglo) == 1);
	assert(stack_push(pila, (arreglo + 9)) == 1);
	printf("DONE!\n");
	printf("It should pop data to the stack reverting the order\n");
	assert(*((int*)stack_pop(pila)) == 10);
	printf("DONE!\n");
	printf("It should tell the count of the data in the stack\n");
	assert(stack_count(pila) == 1);
	assert(*((int*)stack_pop(pila)) == 1);
	printf("DONE!\n");
	
	printf("It should return null when empty\n");
	assert((int*)stack_pop(pila) == NULL);
	printf("DONE!\n");

	stack_free(pila);
	separator();
	return;
}

int main(int argc, char ** argv) {

	stack_test();
	list_test();
	return 0;
}
