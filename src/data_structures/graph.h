/*
 *  graph.h
 *  so-2011
 *
 *  Created by Cristian Pereyra on 03/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include "../includes.h"
#include "list.h"

#ifndef _GRAPH_H_
#define _GRAPH_H_

typedef struct graph_node * node;

typedef struct arc * arc;

typedef struct graph * graph;

// Recibe un comparer por defecto, si es NULL, usa pointer_comparer
// Lo mismo con cloner.
graph graph_init(comparer comp, cloner clon);

// Agrega un nodo al grafo con la clave dada.
int graph_add_node(graph g, void_p key, void_p value);

// Agrega un arco al grafo desde la primera clave hacia la segunda
// Marcando el peso dado.
int graph_add_arc(graph g, void_p from, void_p to, int weight);

// Saca un nodo del grafo, sacando todos los arcos también!!! y freeing!
void_p graph_remove_node(graph g, void_p key);

// Saca un arco del grafo. Hace free de la estructura interna
int graph_remove_arc(graph g, void_p from, void_p to);

// Devuelve los punteros a las claves del grafo
void_p * graph_keys(graph g);

// Devuelve los punteros a los nodos almacenados en el grafo.
list	graph_nodes(graph g);

// Devuelve el nodo de la clave dada
node graph_get_node(graph g, void_p key);


// Graph_node debería contener la clave del nodo
// + el puntero a lo que almacena + la lista de arcos.

// Devuelve la clave del nodo
void_p		graph_node_key(node n);
// Devuelve el valor del nodo
void_p		graph_node_value(node n);
// Devuelve los arcos del nodo
list		graph_node_arcs(node n);

// Arc debería contener un int y un puntero a un nodo.
// Devuelve el peso del arco asociado
int 		graph_arc_weight(arc a);
// Devuelve el puntero al nodo del arco asociaco.
node 		graph_arc_to(arc a);

// Libera el grafo.
void graph_free(graph g);

#endif