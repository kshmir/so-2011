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
#include "utils/colors.h"
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>

#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#define NINJA_GROUP 0xfe0

#define	SOCKET_DEBUG 0
#define	SMEM_DEBUG 0
#define	MSGQ_DEBUG 0
#define	PIPE_DEBUG 0

#define READ 0x01
#define	WRITE	0x02

#define TRUE 1
#define FALSE 0
#include <stdio.h>
#include <stdlib.h>
#define TIMEOUT 250 * 1000 * 1000


// it'd be cool to have this as a variable
#define USE_COLORS 1


typedef enum {
	NEGRO        ,
	ROJO         ,
	VERDE        ,
	MARRON       ,
	AZUL         ,
	VIOLETA      ,
	CELESTE      ,
	GRIS_CLARO   ,
	GRIS         ,
	ROSA         ,
	VERDE_CLARO  ,
	AMARILLO     ,
	AZUL_CLARO   ,
	VIOLETA_CLARO,
	CELESTE_CLARO,
	BLANCO
	
} color;

typedef enum {
	CLEAR        ,
	NONE         ,
	
	NEGRITA      ,
	SUBRAYADO    ,
	BLINK        ,
	INVERTIDO    ,
	
	NO_NEGRITA   ,
	NO_SUBRAYADO ,
	NO_BLINK     ,
	NO_INVERTIDO
	
} attr;

// void pointer for all our solutions
typedef void * void_p;

// generic comparer for generics implementation
typedef int (*comparer)(void_p, void_p);

typedef void (*printer)(void_p);

typedef void_p (*cloner)(void_p);

#define Error( Str )        FatalError( Str )
#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )

int pointer_comparer(void_p int1, void_p int2);

int int_comparer(void_p int1, void_p int2);

int double_comparer(void_p double1, void_p double2);

int cstring_ds_comparer(void_p s1, void_p s2);

int cstring_comparer(void_p s1, void_p s2);

void int_printer(void_p int1);

void double_printer(void_p double1);

void cstring_printer(void_p s1);

void_p pointer_cloner(void_p);

void_p int_cloner(void_p int1);

void_p double_cloner(void_p double1);

void_p cstring_cloner(void_p s1);

#define OK		1
#define DEBUG	2
#define	ERROR	3

#define _OKo "\033[0;32m"
#define _OKc "\033[0m"
#define _DEBUGo "\033[0;34m"
#define _DEBUGc "\033[0m"
#define _ERRORo "\033[0;31m"
#define _ERRORc "\033[0m"

#define	SMEM_DEBUG	0
#define	SOCK_DEBUG	0
#define	PIPE_DEBUG	0
#define	MSGQ_DEBUG	0

#define	READ	0x01
#define	WRITE	0x02

void cprintf(char * format, int color, ...);

void tp1_disclaimer();

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, void_p ftwbuf);

void _catch(int sig);

void _catch_child(int sig);

#endif
