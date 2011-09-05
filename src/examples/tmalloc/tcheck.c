// tcheck.c

#include <malloc.h> // for malloc and friends when not using "tmalloc.h"
#include "tmalloc.h"

void foo(void)
{
	char * s = malloc(200);
}
