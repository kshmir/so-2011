/*
 *  map.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 06/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include "../includes.h"
#include "tree.h"
#include "list.h"
#include "map.h"

// This map works as a treemap, it's implementation uses a tree from behind
// in order to make fast insertions and deletions of data.
// And at the same time it can retrieve the data sorted in O(n)

void struct_data_int_printer(void_p int1) {
	printf("%d",**((int**)int1));
}

// This is the data container which is sent to the tree
struct data {
	void_p key;
	void_p value;
} typedef data;

// The actual map definition
struct map {
	tree t;
	cloner clon_function;
	int	comparer_index;
};

// This data is used to store the comparers of each map.
// It would be cool to change this for something better...
// Or at least shrink this array!!! #TODO
static int comparers = 0;
static int c_index = -1;
static comparer * comparer_storage = NULL;

// Since it uses c_index, this should be locked.
// Compares by the given key.
int compare_data(void_p id, void_p data_struct) {
	void_p id2 = (void_p)((data*) data_struct)->key;
	void_p id1 = (void_p)((data*) id)->key;
	return comparer_storage[c_index](id1,id2);
}

// Starts the map and stores the cloner and comparer
map map_init(comparer comp, cloner clon_function) {
	map m = malloc(sizeof(struct map));
	tree t = tree_init(compare_data);
	m->t = t;
	m->comparer_index = comparers;
	// The map copies the keys if it has a function to do so
	if (clon_function != NULL)
		m->clon_function = clon_function;
	else {
		m->clon_function = NULL;
	}
	// Builds the comparer array for each new map.
	// It could have a threshold...
	if (comparer_storage == NULL) {
		comparer_storage = (comparer*)malloc(sizeof(comparer));
		*comparer_storage = comp;
	} else {
		comparer * old = comparer_storage;
		comparer_storage = (comparer*) malloc(sizeof(comparer) * (comparers + 1));
		int i = 0;
		for (; i < comparers; i++) {
			comparer_storage[i] = old[i];
		}
		comparer_storage[i] = comp;
	}
	comparers++;

	return m;
}

// This method and remove should be lockable!!!
int map_set(map m, void_p key, void_p value) {
	
	data * d = (data*) malloc(sizeof(data));
	if (m->clon_function)
		d->key = m->clon_function(key);
	else {
		d->key = key;
	}
	d->value = value;
	c_index = m->comparer_index;
	if (tree_add(m->t, d)){
		tree_print(m->t,struct_data_int_printer); // This should be lockable or won't handle 
		return 1;			
	}	// multiple threads!!!!!!!!!!
	else {
		free(d);
		return 0;
	}

}

// This method should be locked
void_p map_get(map m, void_p key) {
	data * finder = (data*) malloc(sizeof(data));
	finder->key = key;
	c_index = m->comparer_index;
	data * d = (data *)tree_get(m->t, finder);
	free(finder);
	if (d != NULL)
		return d->value;
	else {
		return NULL;
	}

}

// This method should be locked
void_p map_remove(map m, void_p key) {
	data * finder = (data*) malloc(sizeof(data));
	finder->key = key;
	c_index = m->comparer_index;
	data * d = (data *)tree_delete(m->t, finder);	
	free(finder);	
	void_p ret = d->value;
	if (m->clon_function) {
		free(d->key);
	}
	free(d);
	return ret;
}

// This method shuould be locked
// It makes an array pointing to all the keys in the map.
// You really shouldn't modify it's values...
list map_keys(map m) {
	int i = 0;
	list datas = tree_to_list(m->t);
	list l = list_init();
	for (i=0; i < tree_size(m->t); i++) {
		list_add(l, ((data*)list_get(datas, i))->key);
	}
	return l;
}

// This method shuould be locked
// It makes an array pointing to all the values in the map.
// You can modify the values of this map.
list map_values(map m) {
	int i = 0;
	list datas = tree_to_list(m->t);
	list l = list_init();
	for (i=0; i < tree_size(m->t); i++) {
		list_add(l, ((data*)list_get(datas, i))->value);
	}
	return l;
}

// Tells the size of the map
int map_size(map m) {
	return tree_size(m->t); // Actually of it's tree ;)
}

// Frees the map
void map_free(map m) {
	tree_free(m->t);
	free(m);
}