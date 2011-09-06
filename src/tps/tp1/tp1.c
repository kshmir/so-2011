#include "app/sim_frontend.h"
#include "sim_common_reqs.h"

#include <sys/types.h>
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include <signal.h>


void handler(int sig) {
	void *array[10];
	size_t size;
	
	// get void*'s for all entries on the stack
	size = backtrace(array, 10);
	
	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, 2);
	exit(1);
}



int main(int argc, char ** params) {
	signal(SIGSEGV, handler);   
	srand(time(NULL));
	signal(SIGKILL, &_catch);
	signal(SIGINT, &_catch);
	list args = list_from_ptrarray_w_count(argc, sizeof(char**), params);
	
	nftw("./tmp",  (void_p) unlink_cb, 64, 0);
	list_remove(args, 0);
	setpgid(0, 0);
	
	sim_frontend_main(args);
	
	printf("Freein' tmp...\n");
	free_prints_sem();
	nftw("./tmp",  (void_p) unlink_cb, 64, 0);
	shm_delete();	
	clear_msgq();
	list_free(args);
	killpg(0, SIGINT);  
	return 0;
}