#include "app/sim_frontend.h"
#include "sim_common_reqs.h"

int main(int argc, char ** params) {
	
	printf("SISTEMAS OPERATIVOS - ITBA - 2011\n\n");
	printf("Alumnos\n");
	printf("\tMARSEILLAN, AGUSTIN\n");
	printf("\tPEREYRA, CRISTIAN\n");
	printf("\tVIDELA, MAXIMO\n");
	printf("Launching frontend...\n");
	
	list args = list_from_ptrarray_w_count(argc, sizeof(char**), params);
	
	sim_frontend_main(args);
	
	list_free(args);
	
	printf("I am closing now... bye!\n");
	return 0;
}