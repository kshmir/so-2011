/*
 *  map.h
 *  so-2011
 *
 *  Created by Cristian Pereyra on 06/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include "../includes.h"
#include "list.h"

#ifndef _MAP_H_ 
#define _MAP_H_

// This makes all calls to map equal to "struct map *"
// So BEWARE that any call to map is a pointer by itself.
typedef struct map * map;

// Inits the map
map map_init(comparer comp, cloner c);

// Sets a key in the map
int map_set(map m, void_p key, void_p value);

// Removes a key in the map and returns the pointer to the value.
void_p map_remove(map m, void_p key);

// Gets a value from a key
void_p map_get(map m, void_p key);

// Gets all the keys of the map
list map_keys(map m);

// Gets all the values of the map
list map_values(map m);

// Gets the size of the map
int map_size(map m);

// Frees up the map
void map_free(map m);

#endif