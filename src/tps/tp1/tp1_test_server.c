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

#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"


void separator() {
	printf("--------------------------------\n");
}
sim_client glob = NULL;
int quit = 0;
pthread_cond_t	conds;
pthread_mutex_t mutex;

void networking_test_receiver(sim_message mes) {
	cstring message = sim_message_read(mes);
	assert(cstring_compare(message, "hello client") == 0);
	sim_message_set_header(mes, "0 PRINT ");
	sim_message_write(mes, "hello server");
	sim_message_respond(mes);
	pthread_cond_signal(&conds);
}



void networking_test(connection_type conn, int from_id, int to_id) {
	srand(time(NULL));
	// Starts a client and makes a lot of noise.
	
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&conds,NULL);
	sim_client c = sim_client_init(conn, 0, from_id, to_id, networking_test_receiver);
	
	int i = 0;
	for(; i < to_id; i++)
		rand();
	
	int j = rand() % 3 + 1;
	cstring text = cstring_fromInt(to_id);
	text = cstring_write(text, cstring_fromInt(j));

	sleep(j);
	glob = c;
	sim_client_print(c,text, -1); 
	

	pthread_cond_wait(&conds, &mutex);
	
	// Stops sending noise to the server.
}

int main(int argc, char ** params) {
	if (argc == 4) {	
		int parsed_1 = 0;
		int connection_t = cstring_parseInt(params[1], &parsed_1);
		int parsed_2 = 0;
		int from_id = cstring_parseInt(params[2], &parsed_2);
		int parsed_3 = 0;
		int to_id = cstring_parseInt(params[3], &parsed_3);
		if (parsed_1 && parsed_2 && parsed_3) {
			networking_test(connection_t, from_id, to_id);
		}
	}
	return 0;
}