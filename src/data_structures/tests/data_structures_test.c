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

#include "../../includes.h"
#include "../stack.h"
#include "../list.h"
#include "../tree.h"
#include "../map.h"
#include "../graph.h"

void separator() {
	printf("------------------------------------\n");	
}

void list_test() {
	separator();
	printf("Testing list repeated elements\n");
	
	list lista = list_init();
	
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
	
	printf("I should be able to remove elements\n");
	for (i=0; i < 3; i++) {
		assert(list_add(lista, array + i) > 0); 
	}
	printf("DONE!\n");
	
	list_free(lista);
	free(array);
	separator();
	return;
}

void stack_test() {
	separator();
	
	printf("Testing stack\n");
	stack pila = stack_init(1);
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

void tree_test() {
	separator();
	printf("Testing AVL Tree ADT\n");
	tree t = tree_init(int_comparer);
	
	printf("Should start with no elements\n");
	assert(tree_size(t) == 0);
	printf("DONE!\n");
	
	printf("Should be able to insert a lot of data\n");
	int i = 0;
	for (; i < 1024; i++) {
		int * data = (int*) malloc(sizeof(int));
		*data = i;
		assert(tree_add(t, data) == 1);
	}
	printf("DONE!\n");
	
	int a = -1;
	
	printf("It should find data\n");
	assert(tree_get(t,&a) == NULL);
	a = 10;
	assert(*(int*)tree_get(t,&a) == a);
	printf("DONE!\n");
	
	printf("Should be able to delete a lot of data and get it back\n");
	for (i = 0; i < 1024; i++) {
		int * ptr = (int *) tree_delete(t, &i);
		assert(ptr != NULL);
		free(ptr);
	}
	printf("DONE!\n");
	
	printf("Should end with no elements\n");
	assert(tree_size(t) == 0);
	printf("DONE!\n");
	
	tree_free(t);
	separator();
}

void map_test() {
	separator();
	
	printf("Testing Map ADT\n");
	
	printf("It should init two maps\n");
	// This is because of the internals, testing two is better :)
	map m = map_init(int_comparer, int_cloner);
	map m2 = map_init(int_comparer, int_cloner);
	printf("DONE!\n");
	
	printf("TODO: This should work with threads\n");
	printf("It should be able to add to both\n");
	int i = 0;
	for(i = 0; i < 1024; i++)
	{
		int *key = (int*)malloc(sizeof(int));
		int *val = (int*)malloc(sizeof(int));
		*key = i;
		*val = i * i;
		map_set(m,  key, val);
		map_set(m2, val, key);
	}
	printf("DONE!\n");

	printf("It should be able to get elements\n");
	for(i = 0; i < 1024; i++)
	{
		int *val = map_get(m,&i);
		assert(*val == i * i);
	}
	
	for(i = 0; i < 1024; i++)
	{
		int *val = map_get(m, &i);
		int *val2 = map_get(m2, val);
		assert(*val2 == i);
	}
	printf("DONE!\n");
	
	
	printf("It should be able to print values and keys of the map\n");
	int ** values = (int **) map_values(m);
	for (i = 0; i < 1024; i++) {
		assert(*values[i] == i * i);
	}
	free(values);
	
	int ** keys = (int **) map_keys(m);
	for (i = 0; i < 1024; i++) {
		assert(*keys[i] == i);
	}
	free(keys);
	printf("DONE!\n");
	

	printf("It should be able to remove all keys\n");
	// This should be the way to clean the map if you won't use the values 
	// any longer !!!!!!!!!
	for(i = 0; i < 1024; i++)
	{
		int *val = map_remove(m, &i);
		int *val2 = map_remove(m2, val);
		free(val);
		free(val2);
	}
	printf("DONE!\n");
	
	map_free(m);
	separator();
}

void graph_test(){
	separator();
	
	printf("Testing Graph ADT/n");
	
	printf("Initializing 2 graphs.............");
	graph g1 = graph_init(NULL, NULL);
	graph g2 = graph_init(NULL, NULL);
	printf("DONE!/n");
	
	printf("Adding nodes.............");
	int i = 0;
	for (i = 0; i<10; i++) {
		int *key = malloc(sizeof(int));
		int *value = malloc(sizeof(int));
		*key = i;
		*value = i+10;
		assert(graph_add_node(g1, key, value));
		assert(graph_add_node(g2, value, key));
	}
	int * key = malloc(sizeof(int));
	int * value = malloc(sizeof(int));
	*key = 0;
	*value = 10;
	assert(!graph_add_node(g1, key, value));
	printf("DONE!/n");
	
	printf("Adding arcs.............");
	for (i = 0; i<10; i++) {
		int *key = malloc(sizeof(int));
		int *value = malloc(sizeof(int));
		*key = i;
		*value = i+10;
		assert(graph_add_arc(g1, key, value, i));
		assert(graph_add_arc(g2, value, key, i+100));
	}
	*key = 100;
	*value = 101;
	assert(!graph_add_arc(g1, key, value, 100));
	printf("DONE!/n");
	
	printf("Removing nodes...........");
	printf("DONE!/n");
	
	printf("Removing arcs............");
	printf("DONE!/n");
	
	printf("Testing access functions.");
	printf("DONE!/n");
}

int main(int argc, char ** argv) {
	
	tree_test();		// AVL Tree, for general storage.
	stack_test();		// Stack,	 for algorithms.
	list_test();		// List,	 for manipulating data.
	map_test();			// Map,		 for storing data. (uses tree!)
	//heap_test();		// Heap,     for algorithms / queuing data (proccesses)
	return 0;
}
