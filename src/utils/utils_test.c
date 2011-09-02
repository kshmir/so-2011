/*
 *  utils_test.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 09/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */


#include "cstring.h"
#include "utils_test.h"
#include "../data_structures/list.h"
#include "sem.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void separator() {
	printf("------------------------------------\n");	
}


void sem_test() {
	separator();
	printf("SEMAPHORE TEST\n");
	int s = sem_create(10);
	int s2 = sem_create(10);
	printf("%d %d\n", s, s2);
	sem_free(s,10);
	sem_free(s2,10);
	
	int t = sem_create_typed("text_1");
	int t2 = sem_create_typed("text_1");
	printf("%d %d\n", t, t2);
	sem_free_typed(t, "text_1");	
	sem_free_typed(t, "text_2");
	separator();
}


void cstring_test() {
	separator();
	
	printf("Testing cstrings library.....\n");
	
	printf("Copy, write, write char...\n");
	cstring s = cstring_copy("HOLA!");
	s = cstring_write(s," CHAU!");
	assert(cstring_compare(s, "HOLA! CHAU!") == 0);
	s = cstring_write_c(s,'f');
	assert(cstring_compare(s, "HOLA! CHAU!f") == 0);
	free(s);
	printf("DONE!\n");
	
	printf("Split...\n");
	s = cstring_copy("TEST--STRING-SPLIT--WITH--MANY--PARAMS--AND--EXPAND");
	cstring * splits = cstring_split(s,"--");
	int i = 0;
	for (; splits[i] != 0; i++) {
		if (i == 0)
			assert(cstring_compare(splits[i],"TEST") == 0);
		else if (i == 1)
			assert(cstring_compare(splits[i],"STRING-SPLIT") == 0);
		else if (i == 2)
			assert(cstring_compare(splits[i],"WITH") == 0);
		else if (i == 3)
			assert(cstring_compare(splits[i],"MANY") == 0);
		else if (i == 4)
			assert(cstring_compare(splits[i],"PARAMS") == 0);
		else if (i == 5)
			assert(cstring_compare(splits[i],"AND") == 0);
		else if (i == 6)
			assert(cstring_compare(splits[i],"EXPAND") == 0);
		
		free(splits[i]);
	}
	assert(i == 7);
	free(splits);
	free(s);
	printf("DONE!\n");
	
	printf("ParseInt...\n");
	int ret = 0;
	s = cstring_copy("-456");
	int parsed = cstring_parseInt(s,&ret);
	free(s);
	assert(ret == 1);
	assert(parsed == -456);
	
	
	ret = 0;
	s = cstring_copy("2355");
	parsed = cstring_parseInt(s,&ret);
	free(s);
	assert(ret == 1);
	assert(parsed == 2355);
	
	ret = 0;
	s = cstring_copy("2355a");
	parsed = cstring_parseInt(s,&ret);
	free(s);
	assert(ret == 0);
	
	printf("DONE!...\n");
	
	cstring joins[4];
	joins[0] = "HOLA";
	joins[1] = "COMO";
	joins[2] = "ANDAS";
	joins[3] = 0;

	
	printf("String Join, reverse, fromInt, replace, join list..\n");
	
	s = cstring_join(joins," - ");
	assert(cstring_compare(s,"HOLA - COMO - ANDAS") == 0);
	free(s);
	s = cstring_copy("HOLA");
	s = cstring_reverse(s);
	assert(cstring_compare(s,"ALOH") == 0);
	free(s);
	s =	cstring_fromInt(50002);
	assert(cstring_compare(s, "50002") == 0);
	free(s);
	s =	cstring_fromInt(0);
	assert(cstring_compare(s, "0") == 0);
	free(s);
	s = cstring_replace("HOLA CRISTIAN COMO ANDAS", " ", "");
	assert(cstring_compare(s,"HOLACRISTIANCOMOANDAS") == 0);
	free(s);
	cstring * lineas = cstring_split("hola\n\nchau","\n");
	for (i = 0; lineas[i] ; i++) {
		free(lineas[i]);
	}
	free(lineas);

	assert(i == 3);
	
	list l = cstring_split_list("HOLA_CRISTIAN_COMO_ANDAS", "_");

	
	
	
	assert(list_size(l) == 4);
	assert(strcmp(list_get(l, 0), "HOLA") == 0);
	list_free_with_data(l);
	
	
	printf("DONE...\n");
	

	
	separator();
}


int main(int argc, char ** argv) {
	printf("NONE OF THE FOLLOWING TESTS ARE GUARANTEED TO PASS ON ALL PLATFORMS\n");
	printf("THEY ARE USED AS ANOTHER DEVELOPMENT TOOL AND THEY ***ARE NOT*** MEANT\n");
	printf("TO BE EFFICIENT OR STABLE. THEY ARE MEANT TO GUARANTEE THAT THE TP CAN WORK\n");
	
//	cstring_test();
	sem_test();
	return 0;
}