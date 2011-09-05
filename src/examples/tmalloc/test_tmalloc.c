// test_tmalloc.c
//
// You may need to change a few things depending on your system.
// It shouldn't be difficult as there is not much to this library.
//
#include <windows.h>
#include <malloc.h>  // for malloc and friends
#include "tmalloc.h" // tmalloc should be included after malloc.h
#include "tcheck.h"  // tests a call to malloc in check.c

//----------------------------------------------------------------
// Make project with 'test_tmalloc.c', 'tmalloc.c' and 'tcheck.c'
// The name of translation unit is stored and will be truncated to 30 chars.
//------------------------------------------
// Including tmalloc.h redefines malloc and friends.
//
//  malloc()  -> tmalloc()
//  calloc()  -> tcalloc()
//  realloc() -> trealloc()
//  free()    -> tfree()
//
// When testing is finished do not include tmalloc.h, then all functions will be
// normal again.
//-------------------------------------------

int main( void )
{
	// These call tmalloc and tcalloc if tmalloc.h is included
	char * a = malloc( 1000 );
	char * b = calloc( 1, 2000 );
	char * c = malloc( 1000 );

#ifdef _TMALLOC_
	report_heap();	// show the heap so far
#endif

	// malloc in another translation unit, tcheck.c
	// this is never free-ed and will be reported as such
	// with the last call to report_heap().
	foo();

	b = realloc( b, 3000 );

#ifdef _TMALLOC_
	// show the heap so far
	report_heap();
#endif

	free( a );
	realloc( b, 0 );					// free
	free( c );
	char *ff ;

	// ordinary malloc
	char * d = realloc( NULL, 3000 );
	free(d);

	// try to free an already free-ed block
	d = realloc( d, 0 );

#ifdef _TMALLOC_
	// Final heap report showing what has not been free-ed.
	// The memory address, type of allocation, size of allocated memory,
	// line number where allocation was made, and the translation unit
	// are displayed.
	report_heap();
#endif

    return 0;
}


