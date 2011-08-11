/*
 *  includes.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 05/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *1
 */

#include "includes.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int pointer_comparer(void_p int1, void_p int2) {
	return (int*)int1 - (int*)int2;
}

int int_comparer(void_p int1, void_p int2) {

	if ((int)int2 == 0)
		return 1;

	if ((int)int1 == 0)
		return -1;
	return *(int*)int1 - *(int*)int2;
}

int double_comparer(void_p double1, void_p double2) {
	if (*((double*)double1) < *((double*)double2)) {
		return -1;
	}
	else if (*((double*)double1) > *((double*)double2)) {
		return 1;
	}
	else {
		return 0;
	}
}

int cstring_comparer(void_p s1, void_p s2) {
	return strcmp(*(char**)s1, *(char**)s2);
}


void int_printer(void_p int1) {
	printf("%d",*((int*)int1));
}
void double_printer(void_p double1){
	printf("%g",*((double*)double1));
}

void cstring_printer(void_p s1) {
	printf("%s",(char*)s1);
}

void_p pointer_cloner(void_p p1) {
	return int_cloner(p1); //x86 only!
}

void_p int_cloner(void_p int1) {
	int * nuevo = (int *) malloc(sizeof(int));
	*nuevo = *(int*)int1;
	return nuevo;
}
void_p double_cloner(void_p double1){
	double * nuevo = (double *) malloc(sizeof(double));
	*nuevo = *(int*)double1;
	return nuevo;
}

void_p cstring_cloner(void_p s1) {
	char * _s1 = * (char **) s1;
	int len = strlen(_s1);
	char ** nuevo = (char **)malloc(sizeof(char*) * (len+1));
	*nuevo = strcpy(*nuevo, _s1);
	return nuevo;
}