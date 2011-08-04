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
#include "../list.h"

int main(void)	{

	printf("Testing list without repeated elements\n");
	
	list * lista = list_init(NULL);
	
	printf("It should start the list\n");
	assert(lista != NULL);
		
	int * array = malloc(sizeof(int) * 10);
	int i = 0;
	for (; i < 10; i++) {
		array[i] = i;
	}

	printf("Adding an element should return 1\n");
	for (i=0; i < 10; i++) {
		assert(list_add(lista, array + i) == 1);
	}
	
	printf("It should increase the size of the list\n");
	assert(lista->size == 10);
	
	printf("I should get the value with an index from 0 to n-1\n");
	assert(*((int*)list_get(lista, 4)) == 4);		
	assert(*((int*)list_get(lista, 0)) == 0);		
	assert(*((int*)list_get(lista, 9)) == 9);		
	
	printf("I should get null if the index is not between 0 to n-1\n");
	assert(list_get(lista, -1) == NULL);		
	assert(list_get(lista, 11) == NULL);		
	
	printf("I should be able to insert between 0 to n\n");
	//assert(list_insert(lista, 11, algunvalor) == NULL);		
	
	list_free(lista);
	free(array);
	return 0;
}
