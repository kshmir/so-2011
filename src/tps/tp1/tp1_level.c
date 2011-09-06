#include "sim_common_reqs.h"
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


int main(int argc, char **params) {
	signal(SIGSEGV, handler);   
	srand(time(NULL));
	signal(SIGINT, &_catch_child);
	signal(SIGKILL, &_catch_child);
	if (argc == 4) {	
		int parsed_1 = 0;
		int parsed_2 = 0;
		int parsed_3 = 0;
		
		int connection_t = cstring_parseInt(params[1], &parsed_1);
		int from_id = cstring_parseInt(params[2], &parsed_2);
		int to_id = cstring_parseInt(params[3], &parsed_3);
		
		if (parsed_1 && parsed_2 && parsed_3) {				
			printf("Starting LEVEL...\n");
			sim_level_main(connection_t, from_id, to_id);
		}
	}
	nftw("./tmp",  (void_p) unlink_cb, 64, 0);
	return 0;
}