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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void separator() {
	printf("------------------------------------\n");	
}

typedef int (*function)();

cstring build_string(cstring a, cstring b) {
	return cstring_write(a,b);
}


void cstring_test() {
	separator();
	
	printf("Testing cstrings library.....\n");
	
	printf("Copy, write, write char...\n");
	cstring s = cstring_copy("HOLA!");
	assert(cstring_compare(cstring_write(s," CHAU!"), "HOLA! CHAU!") == 0);
	assert(cstring_compare(cstring_write_c(s,'f'), "HOLA! CHAU!f") == 0);
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
	}
	assert(i == 7);
	printf("DONE!\n");
	
	printf("ParseInt...\n");
	int ret = 0;
	int parsed = cstring_parseInt(cstring_copy("-456"),&ret);
	assert(ret == 1);
	assert(parsed == -456);
	
	
	ret = 0;
	parsed = cstring_parseInt(cstring_copy("2355"),&ret);
	assert(ret == 1);
	assert(parsed == 2355);
	
	ret = 0;
	parsed = cstring_parseInt(cstring_copy("2355a"),&ret);
	assert(ret == 0);
	
	printf("DONE!...\n");
	
	cstring joins[4];
	joins[0] = "HOLA";
	joins[1] = "COMO";
	joins[2] = "ANDAS";
	joins[3] = 0;

	
	printf("String Join, reverse, fromInt, replace..\n");
	
	assert(cstring_compare(cstring_join(joins," - "),"HOLA - COMO - ANDAS") == 0);
	assert(cstring_compare(cstring_reverse(cstring_copy("HOLA")),"ALOH") == 0);
	assert(cstring_compare(cstring_fromInt(50002), "50002") == 0);
	assert(cstring_compare(cstring_replace("HOLA CRISTIAN COMO ANDAS", " "),"HOLACRISTIANCOMOANDAS") == 0);
	cstring * lineas = cstring_split("hola\n\nchau","\n");
	for (i = 0; lineas[i] ; i++) {
	}
	assert(i == 3);
	
	printf("DONE...\n");
	

	
	separator();
}


int main(int argc, char ** argv) {
	cstring_test();
	
	function f = build_string;
	
	cstring hola = f(cstring_copy("HOLA "), cstring_copy("ABUELA"));
	
	printf("%s",hola);
	
	return 0;
}