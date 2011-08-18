/*
 *  cstring.h
 *  so-2011
 *
 *  Created by Cristian Pereyra on 09/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */


#ifndef _CSTRING_H_
#define _CSTRING_H_

#include "../data_structures/list.h"

typedef char * cstring;

cstring cstring_copy(cstring s);

cstring cstring_init(int size);

cstring cstring_fromInt(int number);

cstring cstring_reverse(cstring s);

cstring cstring_expand(cstring s, int extraSize);

int cstring_compare(cstring s1, cstring s2);

cstring cstring_write(cstring original, cstring to_append);

int cstring_len(cstring s);

cstring cstring_write_c(cstring s,char c);

int cstring_matches(cstring string, cstring substr);

cstring cstring_join(cstring * cstrings, cstring s);

cstring * cstring_split(cstring _sub, cstring s);

cstring cstring_replace(cstring original, cstring substr, cstring replacer);

cstring cstring_add_line(cstring s);

cstring cstring_from_file(cstring path);

cstring cstring_sub(cstring s, int len);

cstring cstring_copy_line(cstring s);

list cstring_split_list(cstring sub, cstring s);

cstring cstring_copy_until_char(cstring str, char c);

void cstring_remove(cstring s, char c);

int cstring_parseInt(cstring s, int * return_code);

void cstring_free(cstring s);

#endif