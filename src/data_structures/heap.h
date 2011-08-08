/*
 *  heap.h
 *  so-2011
 *
 *  Created by Cristian Pereyra on 07/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#ifndef _HEAP_H_
#define _HEAP_H_

struct HeapStruct;
typedef struct HeapStruct * heap;




heap heap_init(int max_elements, comparer comp);

void heap_clear(heap H);

void heap_insert(void_p X, heap H);

void_p heap_remove_min(heap H);

void_p heap_get_min(heap H);

int heap_empty(heap H);

int heap_full(heap H);

heap heap_expand(heap h);

void heap_free(heap H);


#endif

