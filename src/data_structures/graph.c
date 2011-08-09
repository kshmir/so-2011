/*
 *  graph.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 03/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "list.h"
#include "map.h"

struct graph{
	map nodes;
	int	comparer_index;
};

struct graph_node {
	void_p key;
	void_p value;
	list arcs;
};

struct arc {
	graph_node to;
	int weight;
};

// This data is used to store the comparers of each map.
// It would be cool to change this for something better...
// Or at least shrink this array!!! #TODO
static int comparers = 0;
static int c_index = -1;
static comparer * comparer_storage = NULL;

// Since it uses c_index, this should be locked.
// Compares by the given key.
static int compare_data(void_p id, void_p data_struct) {
	void_p id1 = ((graph_node) id)->key;
	return comparer_storage[c_index](id,data_struct);
}

graph_node node_init(void_p key, void_p value){
	graph_node ret = malloc(sizeof(struct graph_node));
	ret->key = key;
	ret->value = value;
	ret->arcs = list_init();
	return ret;
}

arc arc_init(graph_node to, int weight){
	arc ret = malloc(sizeof(struct arc));
	ret->to = to;
	ret->weight = weight;
	return ret;
}

graph graph_init(comparer comp, cloner clon){
	graph ret = malloc(sizeof(struct graph));
	ret->nodes = map_init(compare_data, clon);
	ret->comparer_index = comparers;
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
	return ret;
}

int graph_size(graph g)
{
	return map_size(g->nodes);
}

// Agrega un nodo al grafo con la clave dada.
int graph_add_node(graph g, void_p key, void_p value){
	c_index = g->comparer_index;
	graph_node n = node_init(key, value);
	if (map_set(g->nodes, key, n))
		return TRUE;
	else {
		list_free(n->arcs);
		free(n);
		return FALSE;
	}

}

// Agrega un arco al grafo desde la primera clave hacia la segunda
// Marcando el peso dado.
int graph_add_arc(graph g, void_p from, void_p to, int weight){
	c_index = g->comparer_index;
	graph_node _to = (graph_node) map_get(g->nodes, to);
	graph_node _from = (graph_node) map_get(g->nodes, from);
	
	if (_to == NULL || _from == NULL) {
		return FALSE;
	}
	
	
	arc a = arc_init(_to, weight);
	arc b = arc_init(_from, weight);
	if(list_indexOf(_from->arcs, a, int_comparer)!= -1 || 
	   list_indexOf(_to->arcs, b, int_comparer) != -1){
		free(a);
		free(b);
		return FALSE;
	}
	list_add(_from->arcs, a);
	list_add(_to->arcs, b);
	return TRUE;
}

// Saca un nodo del grafo, sacando todos los arcos también!!! y freeing!
void_p graph_remove_node(graph g, void_p key){
	
	// TODO: Make this MUCH clearer. #IDEA: Make a list with comparator insertion and make free's that way.
	// IDEA: Clean up everything
	c_index = g->comparer_index;
	graph_node n = map_get(g->nodes, key);
	list arcs = n->arcs;
	int i = 0;
	int dont_clear = 0;
	for (i = 0; i<list_size(arcs); i++) {
		graph_node _n = ((graph_node)((arc)list_get(arcs, i))->to);
		list newList = _n->arcs;
		c_index = g->comparer_index;
		if (compare_data(key,_n->key) == 0)
			dont_clear = TRUE;
		int j = 0;
		for (j = 0; j<list_size(newList); j++) {
			graph_node aux = ((arc)list_get(newList, j))->to;
			if (aux == n) {
				list_remove(newList, j);
				free(aux);
				dont_clear = TRUE;
			}
		}
		if (dont_clear) {
			free(list_get(arcs, i));
		}
	}
	map_remove(g->nodes, key);

	list_free(arcs);
	if (!dont_clear) {
		free(n->value);
		free(n);
	}
}

// Saca un arco del grafo. Hace free de la estructura interna
int graph_remove_arc(graph g, void_p from, void_p to){
	c_index = g->comparer_index;
	graph_node nFrom = map_get(g->nodes, from);
	graph_node nTo = map_get(g->nodes, to);
	if(nFrom == NULL || nTo == NULL)
		return FALSE;
	int i = 0;
	list arcs = nFrom->arcs;
	for (i = 0; i < list_size(arcs); i++) {
		graph_node aux = ((arc)list_get(arcs, i))->to;
		if (aux == nTo) {
			arc save = list_get(arcs, i);
			list_remove(arcs, i);
			free(save);
			arcs = nTo->arcs;
			for (i = 0; i < list_size(arcs); i++) {
				aux = list_get(arcs,i);
				if(aux == nFrom){
					save = list_get(arcs,i);
					list_remove(arcs, i);
					free(save);
					return TRUE;
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

// Devuelve los punteros a las claves del grafo
list graph_keys(graph g){
	c_index = g->comparer_index;
	return map_keys(g->nodes);
}

// Devuelve los punteros a los nodos almacenados en el grafo.
list graph_nodes(graph g){
	c_index = g->comparer_index;
	return map_values(g->nodes);
}

// Devuelve el nodo de la clave dada
graph_node graph_get_node(graph g, void_p key){
	c_index = g->comparer_index;
	return map_get(g->nodes, key);
}


// Graph_node debería contener la clave del nodo
// + el puntero a lo que almacena + la lista de arcos.

// Devuelve la clave del nodo
void_p		graph_node_key(graph_node n){
	return n->key;
}
// Devuelve el valor del nodo
void_p		graph_node_value(graph_node n){
	return n->value;
}
// Devuelve los arcos del nodo
list		graph_node_arcs(graph_node n){
	return n->arcs;
}

// Arc debería contener un int y un puntero a un nodo.
// Devuelve el peso del arco asociado
int 		graph_arc_weight(arc a){
	return a->weight;
}
// Devuelve el puntero al nodo del arco asociaco.
graph_node 		graph_arc_to(arc a){
	return a->to;
}

// Libera el grafo.
void graph_free(graph g){//TODO ver si map_keys devuelve una lista
	c_index = g->comparer_index;
	list nodes = graph_keys(g);
	int i = 0;
	for (i = 0; i < list_size(nodes); i++) {
		graph_remove_node(g, list_get(nodes,i));
	}
	free(nodes);
	free(g);
}