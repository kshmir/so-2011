/*
 *  The code of this heap is based on this implementation
 *  http://cprogramminglanguage.net/binary-heap-c-code.aspx
 *
 *  Created by Cristian Pereyra on 07/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 */

#include "../includes.h"
#include "heap.h"

#include <stdlib.h>

#define min_size (10)
#define MinData (-32767)

struct HeapStruct {
    int Capacity;
    int Size;
    void_p * Elements;
	comparer Comparer;
};

heap heap_init(int max_elements, comparer comp) {
    heap H;
	
	
	H = (heap) malloc(sizeof( struct HeapStruct));
    if (H == NULL)
		FatalError("Out of space!!!");
	
    H->Elements = (void_p *) malloc((max_elements + 1) * sizeof (void_p));
    if (H->Elements == NULL)
		FatalError("Out of space!!!");
	
    H->Capacity = max_elements;
    H->Size = 0;
    *((int*)H->Elements) = (int)NULL;
	H->Comparer = comp;
	
    return H;
}


/* END */

void heap_clear(heap H) {
    H->Size = 0;
}





// Expands the heap if needed
// It makes it the double of size.
heap heap_expand(heap h) {
	int i = 0;
	
	heap h2 = heap_init(h->Capacity * 2 + 1, h->Comparer);
	
	
	for (; i < h->Capacity + 1; i++) {
		((int*)h2->Elements)[i] = ((int*)h->Elements)[i];
	}
	h2->Size = h->Size;
	
	free(h);
	return h2;
}


/* H->Element[ 0 ] is a sentinel */

void heap_insert(void_p X, heap H) {
    int i;
	int * elements = (int *) H->Elements;
	
    if (H->Size == H->Capacity) {
		return;
    }
	
    for (i = ++H->Size; i/2 > 0 && H->Comparer((void_p)X, (void_p)elements[ i / 2 ]) < 0; i /= 2)
        elements[ i ] = elements[ i / 2 ];
    elements[ i ] = (int) X;
}

/* END */


void_p heap_remove_min(heap H) {
    int i, Child;
    void_p MinElement, LastElement;
	
	int * elements = (int *) H->Elements;
	
    if (heap_empty(H)) {
        return (void_p) elements[ 0 ];
    }
	MinElement = (void_p) elements[ 1 ];
    LastElement = (void_p) elements[ H->Size-- ];
	
    for (i = 1; i * 2 <= H->Size; i = Child) {
        Child = i * 2;
        if (Child != H->Size && H->Comparer((void_p)elements[ Child + 1 ], (void_p)elements[ Child ]) <  0)
			Child++;
		
		if (H->Comparer(LastElement,(void_p)elements[ Child ]) > 0)
			elements[ i ] = elements[ Child ];
        else
			break;
    }
	elements[ i ] = (int) LastElement;
	return MinElement;
}

void_p heap_get_min(heap H) {
	int * elements = (int *) H->Elements;
    if (!heap_empty(H))
        return (void_p)elements[ 1 ];
    Error("Priority Queue is Empty");
    return  (void_p)elements[ 0 ];
}

int heap_empty(heap H) {
    return H->Size == 0;
}

int heap_full(heap H) {
    return H->Size == H->Capacity;
}

void heap_free(heap H) {
    free(H->Elements);
    free(H);
}
