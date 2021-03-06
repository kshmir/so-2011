#include "app/sim_frontend.h"
#include "sim_common_reqs.h"

#include <sys/types.h>
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include <signal.h>


int main(int argc, char ** params) {
	signal(SIGBUS, segfault_handler);   
	signal(SIGSEGV, segfault_handler);   
	srand(time(NULL));
	signal(SIGINT, &_catch);
	list args = list_from_ptrarray_w_count(argc, sizeof(char**), params);
	
	list_remove(args, 0);
	setpgid(0, 0);
	
	sim_frontend_main(args);
	
	list_free(args);
	clean_exit();
	
	exit(0);
}