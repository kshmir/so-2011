/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 *
 * @file sim_frontend.c
 *
 * @brief Internal logic of the frontend and it's validator.
 *
 * @author Agustin Marseillan
 * @author Maximo Videla
 * @author Cristian Pereyra
 *
 */

#include "sim_frontend.h"

/**
	Server used to communicate with the level.
 */
sim_server print_server;

/**
	Level built from the file specified
 */
sim_level  level;

/**
	Airlines built from the files specified.
 */
list	   airlines;


/**
	Semaphore used to wait for the level.
 */
int		   frontend_sem = 0;

/**
	Semaphore used to sync with the level
 */
int		   level_sem = 0;


/**
 * Prints the data received from the level.
 *
 * @param data cstring to print.
 */
void sim_frontend_print(cstring data) {
	cprintf("FRONTEND: %s\n", ROJO,data);
}

/**
 * Gets a message and prints it to the frontend
 * Kinda deprecated... slowed everything up!
 *
 * @param sim_message Request data.
 */
void sim_frontend_receiver(sim_message mes) {
	cstring data = sim_message_read(mes);
	sim_frontend_print(data);
	free(data);
	sim_message_free(mes);
}

/**
 * Sends the level serialized to the level process.
 *
 * @param sim_message Request data.
 */
void sim_frontend_copy_level(sim_message mes) {
	cstring l = sim_level_serialize(level);
	sim_message_write(mes, l);
	sim_message_respond(mes);
	free(l);
}

/**
 * Sends the requested airline to the level process.
 *
 * @param sim_message Request data.
 */
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

/**
 * Binds the server actions and sets ready to talk to it's clients.
 *
 * @param t IPC used internally.
 */
void sim_frontend_start_server(connection_type t) {
	print_server = sim_server_init(t, P_LEVEL, 1);

	char * seq = "PRINT";
	sim_server_add_receiver(print_server, seq, sim_frontend_receiver);
	char * seq2 = "COPY_LEVEL";
	sim_server_add_receiver(print_server, seq2, sim_frontend_copy_level);
	char * seq3 = "COPY_AIR";
	sim_server_add_receiver(print_server, seq3, sim_frontend_copy_airline);

	frontend_sem = sem_create_typed("frontend");
	level_sem    = sem_create_typed("level");
}

/**
 * Starts the level process and locks to receive data.
 */
void sim_frontend_start_processes() {
	sim_server_spawn_child(print_server);
	cprintf("FRONTEND: LEVEL STARTED\n", VERDE);
	sem_down(frontend_sem, 1);					
	cprintf("FRONTEND: SHUTDOWN\n", VERDE);
}


/**
 * Starts the frontend logic, which validates the data and starts the level.
 *
 * @param params	list of executable parameters.
 */
void sim_frontend_main(list params) {
	
	// Shows data of the usage and authors of the TP.
	tp1_disclaimer();
	tp1_usage();

	
	// Data for validating
	connection_type * c_type = (connection_type *) malloc(sizeof(connection_type));
	* c_type = -1;
	cstring level_file = NULL;
	list	airline_files = list_init();
	cstring error_string = NULL;
	int line_error=0;

	// If the params received are correct then it parses all the files.
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
				// If there are no parse errors the server starts
				sim_frontend_start_server(*c_type);
				level = lev;
				airlines = _airlines;
				sim_frontend_start_processes();
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
		
		if (lev != NULL) {
			sim_level_free(lev);
		}

		foreach_(sim_airline, airline, _airlines) {
			sim_airline_free(airline);
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

	sem_free_typed(frontend_sem, "frontend");

	free(c_type);

}
