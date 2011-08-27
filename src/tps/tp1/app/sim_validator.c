#include "sim_validator.h"


#define PARAM_METHOD	0
#define PARAM_LEVEL		1
#define PARAM_AIRLINE	2


static char validator_params[3][10] = {"--method", "--level", "--airline"};

static char connection_params[4][15] = {"fifos", "message_queues", "shared_memory", "sockets"};

int validator_int(cstring * str) {
	int i = 0;
	for(; i < 3; i++) {
		if (cstring_compare(* str, validator_params[i]) == 0)
			return i;
	}
	return -1;
}

int connection_int(cstring * str) {
	
	if (str == NULL) { return -1; }
	
	int i = 0;
	for(; i < 4; i++) {
		cstring original = * str;
		
		cstring parsed = cstring_replace(original, "\"", "");
		cstring parsed2 = cstring_replace(parsed, "'", "");
		
		
		if (cstring_compare(parsed2, connection_params[i]) == 0)
		{
			free(parsed);
			free(parsed2);
			return i;	
		}
		
		free(parsed);
		free(parsed2);
	}
	return -1;
}




int sim_validator_params(list args, cstring * return_level, list return_airlines, cstring * error, connection_type * c_type) {
	foreach(cstring *, key, args) {
		switch(validator_int(key)) {
			case PARAM_METHOD:
				foreach_next(key);
				* c_type = connection_int(key);
				if (* c_type == -1) {
					* error = cstring_copy("Error: Invalid IPC mode");
					return -1;
				}
				break;
			case PARAM_LEVEL:
				foreach_next(key);
				if (access(*key, R_OK) == 0) {
					* return_level = cstring_copy(*key);
				} else {
					* error = cstring_copy("Error: Couldn't open input file for level ");
					* error = cstring_write(* error, *key);
					return -1;
				}				
				break;
			case PARAM_AIRLINE:
				foreach_next(key);
				while(key != NULL) {
					foreach_next(key);
					if (access(*key, R_OK) == 0) {
						list_add(return_airlines,cstring_copy(*key));
					} else {
						* error = cstring_copy("Error: Couldn't open input file for airline ");
						* error = cstring_write(* error, *key);
						return -1;
					}
				}
				if (list_size(return_airlines) == 0) {
					* error = cstring_copy("Error: Didn't have extra params for airline files");
					return -1;
				}
				break;
			default:	
				* error = cstring_copy("Error: Invalid param ");
				* error = cstring_write(* error, *key);
				return -1;
		}
	}
	if (list_size(return_airlines) == 0 || * return_level == NULL || * c_type == -1 ) {
		* error = cstring_copy("Not enough data to start program!");
		return -1;
	}
	
	
	return 1;
}