// colors.c
#include "stdbool.h"
#include "colors.h"

#if defined(__unix__)

	#include "unixColors.c"

#elif defined(__win32__)   || defined(__WIN32__) || \
	  defined(win32)       || defined(WIN32)     || \
	  defined(__win32)     || defined(__WIN32)   || \
	  defined(__windows__) || defined(__WINDOWS__)

	#include "winColors.c"

#else

	#include "noColors.c"

#endif
