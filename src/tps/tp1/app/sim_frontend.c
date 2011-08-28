/*
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 *
 *
 */

#include "sim_frontend.h"


void sim_frontend_print(cstring data) {
	printf("%s\n",data);
}

void sim_frontend_receiver(sim_message mes) {
	cstring data = sim_message_read(mes);
	sim_frontend_print(data);
	free(data);
	sim_message_free(mes);
}



int sim_frontend_main(list params) {
	tp1_disclaimer();
	tp1_usage();
	
	connection_type * c_type = (connection_type *) malloc(sizeof(connection_type));
	* c_type = -1;
	cstring level_file = NULL;
	list	airline_files = list_init();
	cstring error_string = NULL;
	
	if (sim_validator_params(params, &level_file, airline_files, &error_string, &c_type) == TRUE) {
		cstring error_file = NULL;
		sim_level lev = (sim_level) sim_validator_level(level_file);
		
		if (lev != NULL) {
			foreach(cstring, file, airline_files) {
				sim_airline airline = (sim_airline) sim_validator_airline(file, lev);
				if (airline == NULL) {
					error_file = file;
					break;
				}
			}
			if (error_file == NULL) {
				// Enter on loop.
			}
			else {
				sim_frontend_print("Error on airline file named:");
				sim_frontend_print(error_file);
			}

			
		} else {
			sim_frontend_print("Error on level file named:");
			sim_frontend_print(level_file);
		}
	}
	else {
		sim_frontend_print("PARAMS ERROR:");
		sim_frontend_print(error_string);
	}
	
	
	list_free_with_data(airline_files);
	
	if (level_file != NULL) {
		free(level_file);
	}
	
	if (error_string != NULL) {
		free(error_string);
	}
	
	free(c_type);
	
}