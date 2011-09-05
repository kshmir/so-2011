// tmalloc.h

#include <windows.h>

//-------------------------------------------------
// It can be difficult to track down memory leaks
// especially in large listings. tmalloc should
// be used for that purpose.
//
// Generally, call report_heap() just before your
// app terminates, this will report any allocated
// memory blocks that were not released, giving
// the file/module name and line number where
// the memory was allocated.
//-------------------------------------------------

#ifndef _TMALLOC_
#define _TMALLOC_

//==============================================
// Function prototypes of the library (do not
// use these, use the macros instead)
//==============================================

// If the pointer passed to tfree() (free) is
// not valid a pointer a MessageBox is displayed
// and the operation (free) will be aborted.
void   tfree( void *, char*, int );

// report_heap() can be used without passing a
// parameter in which case it will print to stdout
// or with a file name, then it will print
// to the opened file. Use without filename with
// console apps and with filename for Windows apps.
//
// When reporting the filename where malloc,
// calloc or realloc was called, only the first
// 30 chars will be used.
void   report_heap( char * pOutFile = NULL );

void * tmalloc( size_t, char*, int );
void * tcalloc( size_t, size_t, char*, int );
void * trealloc( void *, size_t, char*, int );

//==============================================
// Macros for use in user listing
//==============================================
// malloc, calloc, realloc and free are redefined here
// you may need to undef them depending on your system
#define malloc(n)        tmalloc( n, __FILE__, __LINE__ )
#define calloc(m,n)      tcalloc( m, n, __FILE__, __LINE__ )
#define free(n)          tfree( n, __FILE__, __LINE__ )
#define realloc(n, size) trealloc( n, size, __FILE__, __LINE__ )

#endif
