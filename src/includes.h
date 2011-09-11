/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 *
 * @file includes.h
 *
 * @brief Provides functions and definitions shared across all the programs.
 *
 * @author Agustin Marseillan
 * @author Maximo Videla
 * @author Cristian Pereyra
 *
 */
#include "utils/colors.h"
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>

#ifndef _INCLUDE_H_
#define _INCLUDE_H_

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

#define malloc(p)			g_talloc(p)
#define calloc(p,k)			g_tcalloc(p,k)
#define realloc(p,k)		trealloc(p,k)
#define free(p)				tfree(p)


/**
 * Used to print coloured console
 */
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

/**
 * Used to print coloured console
 */
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

/**
 * We didn't like the idea of having that little * everywhere.
 */
typedef void * void_p;

/**
 * Default comparer
 */
typedef int (*comparer)(void_p, void_p);

/**
 * Useful for debugging
 */
typedef void (*printer)(void_p);

/**
 * Generic function for cloning stuff.
 */
typedef void_p (*cloner)(void_p);

#define Error( Str )        FatalError( Str )
#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )


/**
 * Compares two pointers
 *
 * @param int1 first pointer
 * @param int2 second pointer 
 *
 * @return Comparison
 */
int pointer_comparer(void_p int1, void_p int2);

/**
 * Compares two integers
 *
 * @param int1 first integer
 * @param int2 second integer
 *
 * @return Comparison
 */
int int_comparer(void_p int1, void_p int2);

/**
 * Compares two doubles
 *
 * @param double1 first double
 * @param double2 second double
 *
 * @return Comparison
 */
int double_comparer(void_p double1, void_p double2);

/**
 * Compares two strings
 *
 * @param s1 first char *
 * @param s2 second char *
 *
 * @return Comparison
 */
int cstring_comparer(void_p s1, void_p s2);

/**
 * Prints an integer.
 *
 * @param int1 integer to print.
 */
void int_printer(void_p int1);

/**
 * Prints a double.
 *
 * @param double1 double to print.
 */
void double_printer(void_p double1);

/**
 * Prints a string.
 *
 * @param s1 string to print.
 */
void cstring_printer(void_p s1);

/**
 * Clones an integer.
 *
 * @param int1 integer to clone.
 *
 * @return Cloned integer
 */
void_p int_cloner(void_p int1);

/**
 * Clones a double
 *
 * @param double1 double to clone.
 *
 * @return Cloned double.
 */
void_p double_cloner(void_p double1);

/**
 * Clones a cstring (char *)
 *
 * @param s1 cstring to clone.
 *
 * @return Cloned cstring.
 */
void_p cstring_cloner(void_p s1);


/**
 * Prints with color, semaphorized.
 *
 * @param format	printf formatted string.
 * @param color		color to print with.
 */
void cprintf(char * format, int color, ...);

/**
 * Prints the disclaimer of the TP1.
 */
void tp1_disclaimer();

/**
 * Catches SIGNINT
 */
void _catch(int sig);

/**
 * Catches SIGNINT for non-parent processes.
 */
void _catch_child(int sig);

/**
 * Makes a clean exit.
 */
void clean_exit();



#endif
