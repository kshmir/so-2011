#include "app/sim_frontend.h"
#include "sim_common_reqs.h"

#include <sys/types.h>
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include <signal.h>


int main(int argc, char ** params) {
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
	
	list_free(args);
	killpg(0, SIGINT);  
	return 0;
}