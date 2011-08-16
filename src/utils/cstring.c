/*
 *  cstring.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 09/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include "cstring.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

cstring cstring_copy(cstring s){
	cstring ret = cstring_init(strlen(s));
	ret = strcpy(ret,s);
	return ret;
}

cstring cstring_fromInt(int number) {
	cstring s = cstring_init(0);
	if (number == 0) {
		cstring_write_c(s,'0');
	}
		
	if (number < 0) {
		number *= -1;
		cstring_write_c(s,'-');
	}
	
	while(number > 0) { 
		int rem = number % 10;
		number /= 10;
		cstring_write_c(s,'0' + rem);
	}
	return cstring_reverse(s);
}


cstring cstring_init(int size){
	cstring s = (cstring) calloc(sizeof(char), (size + 1));
	return s;
}

cstring cstring_reverse(cstring s) { 
	cstring s2 = cstring_copy(s);
	int len = cstring_len(s);
	int i;
	for (i = 0; i < len; i++) {
		s[i] = s2[len - i - 1];
	}
	cstring_free(s2);
	return s;
}

cstring cstring_expand(cstring s, int extraSize){
	s = realloc(s, (cstring_len(s) + extraSize + 1) * sizeof(char));
	return s;
}

int cstring_compare(cstring s1, cstring s2)
{
	return strcmp(s1,s2);
}

cstring cstring_write(cstring original, cstring to_append) {
	int extra_size = cstring_len(to_append);
	int old_size = cstring_len(original);

	original = cstring_expand(original,extra_size);
	
	int i = old_size;
	for (;i < old_size + extra_size; i++) {
		original[i] = to_append[i - old_size];
	}
	original[i] = 0;
	
	return original;
}

int cstring_len(cstring s)
{
	return strlen(s);
}

cstring cstring_write_c(cstring s,char c) { 
	char arr[2];
	arr[0] = c;
	arr[1] = 0;
	return cstring_write(s,arr);
}

cstring cstring_replace(cstring original, cstring substr) {
	cstring * strings = cstring_split(original, substr);
	cstring response = cstring_join(strings, "");
	int i = 0;
	while(strings[i] != NULL) {
		free(strings[i]);
		i++;
	}
	free(strings);
	return response;
}



cstring cstring_join(cstring * cstrings, cstring s) { 
	int i = 1;
	cstring sub = cstring_copy(cstrings[0]);
	
	while(cstrings[i] != NULL) {
		if (cstring_len(s) > 0) {
			sub = cstring_write(sub,s);
		}
		sub = cstring_write(sub,cstrings[i]);
		i++;
	}
	return sub;
}


cstring * cstring_split(cstring _sub, cstring s) { 
	cstring sub = cstring_copy(_sub);
	sub = cstring_write(sub,s); // Dirty little hack ;)
	cstring * result = (cstring *) calloc(sizeof(cstring), 5);
	
	int size = 0;
	int hits = 0;
	int index = 0;
	void * found = 0;
	while((found = (void *)strstr(sub + index,s)) != NULL) {
		
		if (hits == size) {
			result = (cstring *) realloc(result, sizeof(char*) * (size + 5));
			size += 5;
		}
		
		cstring found_cstring = (cstring)(sub + index);
		int len = (int)found - ((int)sub + index);
		int i = 0;
		result[hits] = cstring_init(len);
		
		for (; i < len; i++) {
			result[hits][i] = found_cstring[i];
		}
		result[hits][i] = 0;
		index += len + cstring_len(s);
		hits++;
	}
	result[hits] = NULL;
	free(sub);
	return result;
}

int cstring_parseInt(cstring s, int * return_code) { 
	int ret = 0;
	int i = 0;
	int minus = 0;
	*return_code = 1;
	while(s[i] != 0) {
		if (s[i] == '-') {
			minus = 1;
			i++;
		}
		
		if (s[i] - '0' < 10 && s[i] - '0' >= 0) { 
			ret *= 10;
			ret += s[i] - '0';
		}
		else {
			* return_code = 0;
			break;
		}
		i++;
	}

	if (minus) {
		ret *= -1;
	}
	return ret;
}

cstring cstring_add_line(cstring s) { 
	return cstring_write(s, "\n");
}

int cstring_matches(cstring str, cstring s) {
	cstring * splitted = cstring_split(str,s);
	int result = 0, index = 0;
	while (splitted[index] != NULL) {
		free(splitted[index]);
		index++;
	}
	if (index == 1) {
		result = 1;
	}
	free(splitted);
	return result;
}


void cstring_free(cstring s) {
	free(s);
}

cstring cstring_from_file(cstring path){
	int fd = open(path, O_RDONLY);
	char buffer[BUFF_SIZE];
	cstring string = cstring_init(0);
	
	while(read(fd, buffer, BUFF_SIZE)){
		cstring_write(string, buffer);
	}
	
	close(fd);
}