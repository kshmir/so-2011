#include "app/sim_frontend.h"
#include "sim_common_reqs.h"

int main(int argc, char ** params) {
	list args = list_from_ptrarray_w_count(argc, sizeof(char**), params);
	
	list_remove(args, 0);
	sim_frontend_main(args);
	
	list_free(args);
	return 0;
}