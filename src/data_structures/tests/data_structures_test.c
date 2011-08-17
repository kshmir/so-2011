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
#include "../heap.h"
#include "../queue.h"

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
	
	int ob = 0;
	printf("I should be able to get elements between 0 to n\n");
	assert(list_indexOf(lista, &ob, int_comparer) != -1);		
	ob = -10;
	assert(list_indexOf(lista, &ob, int_comparer) == -1);		
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
		assert(tree_add(t, data) == 0);
	}
	printf("DONE!\n");
	

	int a = -1;
	
	printf("It should find data\n");
	assert(tree_get(t,&a) == NULL);
	a = 10;
	assert(*(int*)tree_get(t,&a) == a);
	printf("DONE!\n");
	

	
	printf("It be able to make lists of its values\n");
	list values = (list) tree_to_list(t);
	i = 0;
	for (; i < list_size(values); i++) {
		int *value = tree_get(t,&i);
		assert(int_comparer(value,list_get(values, i)) == 0);
	}
	
	
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
		assert(map_set(m,key,val) == 0);
		assert(map_set(m2,val,key) == 0);
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
	
	int x = -100;
	
	printf("It should return NULL if the key is not inside the mapº\n");
	assert(map_get(m, &x) == NULL);
	printf("DONE!\n");
	
	printf("It should be able to print values and keys of the map\n");
	list values = map_values(m);
	for (i = 0; i < 1024; i++) {
		assert(*(int*)list_get(values, i) == i * i);
	}
	free(values);
	
	list keys = map_keys(m);
	for (i = 0; i < 1024; i++) {
		assert(*(int*)list_get(keys, i) == i);
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
	
	printf("Testing Graph ADT\n");
	
	printf("Initializing 2 graphs.............\n");
	graph g1 = graph_init(int_comparer, int_cloner);
	graph g2 = graph_init(int_comparer, int_cloner);
	printf("DONE!\n");
	
	printf("Adding nodes.............\n");
	int i = 0;
	for (i = 0; i<100; i++) {
		int *key = malloc(sizeof(int));
		int *value = malloc(sizeof(int));
		*key = i;
		*value = i;

		assert(graph_add_node(g1, key, value) == TRUE);
		assert(graph_add_node(g2, value, key) == TRUE);
	}
	int * key = malloc(sizeof(int));
	int * value = malloc(sizeof(int));
	*key = 0;
	*value = 10;
	assert(!graph_add_node(g1, key, value));
	printf("DONE!\n");
	
	printf("Adding arcs.............\n");
	for (i = 0; i<100; i++) {
		int *key = malloc(sizeof(int));
		int x = 1001;
		int j = 0;
		*key = i;
		for (j = 0; j < 100; j++) {
			int *value = malloc(sizeof(int));
			*value = j;
			assert(graph_add_arc(g1, key, &x, i) == FALSE);
			
			graph_node n = graph_get_node(g1, key);
			list arcs = graph_node_arcs(n);
			graph_arc a = (graph_arc) arc_init((graph_node)graph_get_node(g1, value), 0);
			if (list_indexOf(arcs, a, graph_arc_comparer) == -1) {
				assert(graph_add_arc(g1, key, value, i) == TRUE);
			}
			else {
				assert(graph_add_arc(g1, key, value, i) == FALSE);
			}
			free(a);
		}

		if(graph_get_node(g2, value)) {
			graph_add_arc(g2, value, &j, i+100);
		}
		else {
			graph_add_arc(g2, value, key, i+100);
		}
	}
	*key = 100;
	*value = 101;
	assert(!graph_add_arc(g1, key, value, 100));
	printf("DONE!\n");
	
	printf("Removing nodes...........\n");


	for (i = 0; i<100; i+=2) {
		int *key = malloc(sizeof(int));
		int *value = malloc(sizeof(int));
		int x = 1001;
		*key = i;
		*value = i;

		graph_remove_node(g1, key);
		graph_remove_node(g2, key);
	}
	
	assert(graph_size(g1) == 50);
	assert(graph_size(g2) == 50);

	printf("DONE!\n");
	
	printf("Removing arcs............\n");
	for (i = 0; i<100; i++) {
		int *key = malloc(sizeof(int));
		int *value = malloc(sizeof(int));
		int x = 1001;
		*key = i;
		*value = i;
		if(graph_get_node(g1, key) != NULL) {
			graph_node n = graph_get_node(g1, key);
			assert(list_size(graph_node_arcs(n)) == 51); // No sé si este assert debería ser 50 o no...
			graph_remove_arc(g1, key, value);
			assert(list_size(graph_node_arcs(n)) == 50);
		}

	}
	printf("DONE!\n");
	
	printf("Testing access functions.\n");
	printf("NOT DONE!\n");
}

void heap_test() {
	separator();
	printf("Testing heap structure\n");
	
	heap h = heap_init(1024,int_comparer);
	
	int datos[1024*1024];
	int i = 0;
	printf("Inserting 1M records and resizing on the go \n");
	for (; i < 1024*1024; i++) {
		datos[i] = 1024*1024 - 1 - i;
		if (heap_full(h)) {
			h = heap_expand(h);
		}
		
		heap_insert(&(datos[i]), h);
	}
	printf("DONE!\n");
	
	printf("Removing 1M records \n");
	i = 0;
	
	for (; i < 1024*1024; i++) {
		int a = *((int*)heap_remove_min(h));
		assert(a == i);
	}
	printf("DONE!\n");
	
	heap_free(h);
	
	separator();
}

void queue_test() {
	separator();

	printf("Testing Queue\n");
	queue q = queue_init();
	int a, b ,c;
	a = 0;
	b = 1;
	c = 2;
	char * s = "HOLA";
	queue_poll(q, s);
	queue_poll(q, &c);
	queue_poll(q, &b);
	queue_poll(q, &a);
	assert((char *)queue_peek(q) == s);
	assert((char *)queue_pull(q) == s);
	assert(*(int*)queue_pull(q) == 2);
	assert(*(int*)queue_pull(q) == 1);
	assert(*(int*)queue_pull(q) == 0);
	assert(queue_empty(q));
	printf("DONE!\n");
	
	separator();
}

int main(int argc, char ** argv) {
	
	tree_test();		// AVL Tree, for general storage.
	stack_test();		// Stack,	 for algorithms.
	list_test();		// List,	 for manipulating data.
	map_test();			// Map,		 for storing data. (uses tree!)
	heap_test();		// Heap,     for algorithms / queuing data (proccesses)
	queue_test();
	graph_test();
	return 0;
}
