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

#define Error( Str )        FatalError( Str )
#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )


#endif
