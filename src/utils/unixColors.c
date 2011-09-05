// unixColors.c
#include <stdio.h>

#include "colors.h"

#define NOCOLOR 0

const int color2font_color[] = 
{ 30,31,32,33,34,35,36,37,90,91,92,93,94,95,96,97 };
const int color2font_bkcolor[] =
{ 40,41,42,43,44,45,46,47,100,101,102,103,104,105,106,107 };
const int attr2font_attr[] =
{ 0,10,1,4,5,7,22,24,25,27 };

void textcolor( color c ){
	if (NOCOLOR) return;
	printf( "\033[%dm", color2font_color[(int)c] );
}

void backcolor( color c ){
	if (NOCOLOR) return;
	printf( "\033[%dm", color2font_bkcolor[(int)c] );
}

void textattr( attr a ){	
	if (NOCOLOR) return;
	printf( "\033[%dm", attr2font_attr[(int)a] );
}

