/*
 *	This is the main definitions file.
 *
 *  includes.h
 *  so-2011
 *
 *  Created by Cristian Pereyra on 05/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */



#ifndef _INCLUDE_H_
#define _INCLUDE_H_

// void pointer for all our solutions
typedef void * void_p;

// generic comparer for generics implementation
typedef int (*comparer)(void_p, void_p);

typedef void (*printer)(void_p);

typedef void_p (*cloner)(void_p);

#define Error( Str )        FatalError( Str )
#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )

int int_comparer(void_p int1, void_p int2);

int double_comparer(void_p double1, void_p double2);

int string_comparer(void_p s1, void_p s2);

void int_printer(void_p int1);

void double_printer(void_p double1);

void string_printer(void_p s1);

void_p int_cloner(void_p int1);

void_p double_cloner(void_p double1);

void_p string_cloner(void_p s1);

#endif
