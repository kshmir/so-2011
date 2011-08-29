#include "sim_validator.h"


#define PARAM_METHOD	0
#define PARAM_LEVEL		1
#define PARAM_AIRLINE	2


static char validator_params[3][10] = {"--method", "--level", "--airline"};

static char connection_params[4][15] = {"fifos", "message_queues", "sockets", "shared_memory"};

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
					if (access(*key, R_OK) == 0) {
						list_add(return_airlines,cstring_copy(*key));
					} else {
						* error = cstring_copy("Error: Couldn't open input file for airline ");
						* error = cstring_write(* error, *key);
						return -1;
					}
					foreach_next(key);
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

sim_level sim_validator_level(cstring path) {
	cstring file_data = cstring_from_file(path);
	
	if (file_data == NULL) {
		return NULL;
	}
	
	sim_level lev = sim_level_deserialize(file_data);
	free(file_data);	
	
	return lev;
}

sim_airline sim_validator_airline(cstring path, sim_level lev) {
	cstring file_data = cstring_from_file(path);
	
	if (file_data == NULL) {
		return NULL;
	}
	
	sim_airline air = sim_airline_deserialize(file_data, -1);
	free(file_data);	
	

	foreach(sim_plane, plane, sim_airline_planes(air)) {
		if (!sim_level_has_city(lev, sim_plane_start_city(plane))) {
//			sim_airline_free(air); TODO!
			return NULL;
		}
	}

	return air;	
}

