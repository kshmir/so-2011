#include "sim_common_reqs.h"

int main(int argc, char ** params) { 
	if (argc == 4) {	
		int parsed_1 = 0;
		int parsed_2 = 0;
		int parsed_3 = 0;
		
		int connection_t = cstring_parseInt(params[1], &parsed_1);
		int from_id = cstring_parseInt(params[2], &parsed_2);
		int to_id = cstring_parseInt(params[3], &parsed_3);
		
		if (parsed_1 && parsed_2 && parsed_3) {
			sim_level_main(connection_t, from_id, to_id);
		}
	}
	return 0;
}