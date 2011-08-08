/*
 *  graph.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 03/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

// TODO: Hacerlo @ilmarse ;)

#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "list.h"
#include "map.h"

struct graph{
	map nodes;
};

struct node{
	void_p value;
	list arcs;
};

struct arc{
	node to;
	int weight;
};

node init_node(void_p value){
	node ret = malloc(sizeof(struct node));
	ret->value = value;
	ret->arcs = list_init();
	return ret;
}

arc init_arc(node to, int weight){
	arc ret = malloc(sizeof(struct arc));
	ret->to = to;
	ret->weight = weight;
	return ret;
}

graph init_graph(comparer comp, cloner clon){
	graph ret = malloc(sizeof(struct graph));
	ret->nodes = map_init(comp, clon);
	return ret;
}

// Agrega un nodo al grafo con la clave dada.
int graph_add_node(graph g, void_p key, void_p value){
	node n = node_init(value);
	return map_set(g->nodes, key, n);
}

// Agrega un arco al grafo desde la primera clave hacia la segunda
// Marcando el peso dado.
int graph_add_arc(graph g, void_p from, void_p to, int weight){
	arc a = init_arc(map_get(g->nodes, to), weight);
	arc b = init_arc(map_get(g->nodes, from), weight);
	if(list_contains(map_get(g->nodes, from), a) || 
	   list_contains(map_get(g->nodes, to), b)){
		free(a);
		free(b);
		return FALSE;
	}
	list_add((map_get(g->nodes, from))->arcs, a);
	list_add((map_get(g->nodes, to))->arcs, b);
	return TRUE;
}

// Saca un nodo del grafo, sacando todos los arcos también!!! y freeing!
void_p graph_remove_node(graph g, void_p key){
	node n = map_get(g->nodes, key);
	list arcs = n->arcs;
	int i = 0;
	for (i = 0; i<list_size(arcs); i++) {
		arc newList = list_get(arcs, i)->to->arcs;
		int j = 0;
		for (j = 0; j<list_size(newList); j++) {
			node aux = list_get(newList, j)->to;
			if (aux == n) {
				list_remove(newList, j);
				free(aux);
			}
		}
		free(list_get(arcs, i));
	}
	map_remove(g->nodes, key);
	free(n->value);
	list_free(arcs);
	free(n);
}

// Saca un arco del grafo. Hace free de la estructura interna
int graph_remove_arc(graph g, void_p from, void_p to){
	node nFrom = map_get(g->nodes, from);
	node nTo = map_get(g->nodes, to);
	if(nFrom == NULL || nTo == NULL)
		return FALSE;
	int i = 0;
	list arcs = nFrom->arcs;
	for (i = 0; i < list_size(arcs); i++) {
		node aux = list_get(arcs, i)->to;
		if (aux == nTo) {
			arc save = list_get(arcs, i);
			list_remove(i);
			free(save);
			arcs = nTo->arcs;
			for (i = 0; i < list_size(arcs); i++) {
				aux = list_get(arcs,i);
				if(aux == nFrom){
					save = list_get(arcs,i);
					list_remove(i);
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
void_p * graph_keys(graph g){
	return map_keys(g->nodes);
}

// Devuelve los punteros a los nodos almacenados en el grafo.
list 	graph_nodes(graph g){
	return map_values(g->nodes);
}

// Devuelve el nodo de la clave dada
graph_node graph_get_node(graph g, void_p key){
	return map_get(g->nodes, key);
}


// Graph_node debería contener la clave del nodo
// + el puntero a lo que almacena + la lista de arcos.

// Devuelve la clave del nodo
void_p		graph_node_key(graph_node n);
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
	list nodes = graph_keys(g);
	int i = 0;
	for (i = 0; i < list_size(nodes); i++) {
		graph_remove_node(g, list_get(nodes,i));
	}
	free(nodes);
	free(g);
}