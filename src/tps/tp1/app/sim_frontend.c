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

sim_server print_server;
sim_level  level;
list	   airlines;

int		   frontend_sem = 0;
int		   level_sem = 0;



void sim_frontend_print(cstring data) {
	printf("%s\n",data);
}

/*
	Start Frontend Receivers
 */
void sim_frontend_receiver(sim_message mes) {
	cstring data = sim_message_read(mes);
	sim_frontend_print(data);
	free(data);
	sim_message_free(mes);
}


void sim_frontend_copy_level(sim_message mes) {
	sim_message_write(mes, sim_level_serialize(level));
	sim_message_respond(mes);
}

void sim_frontend_copy_airline(sim_message mes) {
	cstring data = cstring_copy(sim_message_read(mes));
	int error = 0;
	int airline_index = cstring_parseInt(data, &error);


	if (error) {
		sim_airline air = list_get(airlines, airline_index);
		cstring resp = cstring_init(0);

		if (air != NULL) {
			resp = cstring_write(resp, sim_airline_serialize(air, FALSE));
		}
		else {
			resp = cstring_write(resp, "null");
		}
		sim_message_write(mes, resp);
		sim_message_respond(mes);
	} else {
		printf("error %d\n",airline_index);
	}
	free(data);
}

/*
	End Frontend receivers.
 */

int sim_frontend_start_server(connection_type t) {
	print_server = sim_server_init(t, P_LEVEL, 0);

	char * seq = "PRINT";
	sim_server_add_receiver(print_server, seq, sim_frontend_receiver);
	char * seq2 = "COPY_LEVEL";
	sim_server_add_receiver(print_server, seq2, sim_frontend_copy_level);
	char * seq3 = "COPY_AIR";
	sim_server_add_receiver(print_server, seq3, sim_frontend_copy_airline);

	frontend_sem = sem_create_typed("frontend");
	level_sem    = sem_create_typed("level");
}


int sim_frontend_start_processes(sim_level lev, list airlines) {
	sim_server_spawn_child(print_server);
	cprintf("FRONTEND: Going down to spawn\n", VERDE);
	sem_down(frontend_sem, 1);					
	cprintf("FRONTEND: SHUTDOWN\n", VERDE);
}



int sim_frontend_main(list params) {
	tp1_disclaimer();
	tp1_usage();

	connection_type * c_type = (connection_type *) malloc(sizeof(connection_type));
	* c_type = -1;
	cstring level_file = NULL;
	list	airline_files = list_init();
	cstring error_string = NULL;
	int line_error=0;

	if (sim_validator_params(params, &level_file, airline_files, &error_string, c_type) == TRUE) {
		cstring error_file = NULL;
		sim_level lev = (sim_level) sim_validator_level(level_file,&line_error);
		list _airlines = list_init();
		if (lev != NULL) {
			foreach(cstring, file, airline_files) {
				sim_airline airline = (sim_airline) sim_validator_airline(file, lev,&line_error);
				if (airline == NULL) {
					error_file = file;
					break;
				}
				list_add(_airlines, airline);
			}
			if (error_file == NULL) {
				sim_frontend_start_server(*c_type);
				level = lev;
				airlines = _airlines;
				sim_frontend_start_processes(lev, airlines);

			}
			else {
				cstring aux=cstring_copy("Error on airline file named: ");
				aux=cstring_write(aux,error_file);
				aux=cstring_write(aux," at line number: ");
				aux=cstring_write(aux,cstring_fromInt(line_error));
				sim_frontend_print(aux);
			}


		} else {
			cstring aux=cstring_copy("Error on level file named:");
			aux=cstring_write(aux,level_file);
			aux=cstring_write(aux," at line number: ");
			aux=cstring_write(aux,cstring_fromInt(line_error));
			sim_frontend_print(aux);
		}
	}
	else {
		sim_frontend_print("PARAMS ERROR:");
		sim_frontend_print(error_string);
	}


	//	list_free_with_data(airline_files);

	if (level_file != NULL) {
		free(level_file);
	}

	if (error_string != NULL) {
		free(error_string);
	}

	//sem_free_typed(frontend_sem, "frontend");

	free(c_type);

}
