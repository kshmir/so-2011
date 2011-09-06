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
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>

#include "../networking/sim_client.h"
#include "../networking/sim_message.h"
#include "../networking/sim_server.h"
#include "../networking/sim_transporter.h"
#include "../sim_common_reqs.h"

#ifndef __MACH__
#define CLIENTS_COUNT 32
#else
#define CLIENTS_COUNT 7 // OSX Has no fun with more than this clients on shmem
#endif

void separator() {
	printf("--------------------------------\n");
}



void file_test(){
	printf("%s", cstring_from_file("./testFile"));
}

pthread_cond_t	conds;
pthread_mutex_t mutex;
cstring buffer;

int message_counter = 0;


void print_receiver(sim_message mes) {
	message_counter++;
	
	if (message_counter >= CLIENTS_COUNT) {
		pthread_cond_broadcast(&conds);
	}
}

void receiver(sim_message mes) {
	buffer = cstring_write(buffer,sim_message_read(mes));
}

void networking_test(connection_type c_type) {
	separator();
	buffer = cstring_init(0);
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&conds,NULL);
	
	//printf("Starting transporter test\n");
//	
//	sim_transporter t = sim_transporter_init(c_type, P_TESTER, 0, 1, MODE_READWRITE, TRUE, TRUE);
//	printf("It should be able to listen and write\n");	
//	sim_transporter_write(t, "server");
//
//	cstring data = sim_transporter_listen(t);
//	sim_transporter_dequeue(t);
//	printf("%s\n", data);
//	assert(cstring_len(data) == 10);
//	
//	printf("DONE!\n");
//	
//	separator();
//	
//	printf("Starting message test\n");
//	
//	printf("It should be able to listen and write through a transporter, and get back the data\n");	
//	
//	printf("I send write again\n");
//	sim_message m = sim_message_init(t, "POST", "Write again");
//	sim_message resp = sim_message_send(m);
//	data = sim_message_read(resp);
//	assert(cstring_compare(data, "Hello Baby"));
//	
//	printf("DONE!\n");
//	
//	separator();
//	
//	printf("Starting client test\n");
//	printf("It should be able to have a listener for querying data\n");	
//	sim_client c = (sim_client) sim_client_from_transporter(t, receiver);
//	sleep(1);
//
//
//	sim_client_free(c);
//	
//	printf("DONE!\n");
//
//	printf("Waiting for client closing....\n");	
//	sleep(1);
//	printf("Waiting for client 1....\n");
//	sleep(1);
//	printf("Waiting for client 2....\n");
//	separator();
//	
//	

	printf("Starting server test\n");
	
	sim_server s = sim_server_init(c_type, P_TESTER_SERVER, 0);
	
	cstring data_p = cstring_copy("PRINT ");

	printf("I should receive 128 messages... it takes 3 seconds\n");
	sim_server_add_receiver(s, data_p, (sim_receiver) print_receiver);
	
	int i = 0;
	for (i = 0; i < CLIENTS_COUNT; i++) {
		sim_server_spawn_child(s);
	}
	
	pthread_cond_wait(&conds, &mutex);
	printf("DONE!\n");
	
	message_counter = 0;
	printf("Now I'll send messages to all my clients\n");
	
	sim_server_broadcast_query(s, cstring_copy("hello client"));
	#ifndef __MACH__
		pthread_cond_wait(&conds, &mutex);
	#else
		while (message_counter <= CLIENTS_COUNT - 2);
	#endif
	
	sim_server_free(s);
	
	printf("DONE!\n");

	sleep(1);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&conds);
	
	separator();
}

void serializing_test(){
	
	//create the strings to be tested
	printf("Serializing & deserializing planes:\n");
	cstring plane = cstring_copy("Andorra\ndroga1 1 droga2 2 droga3 3\n");
	cstring airline = cstring_copy("2\n\nAndorra\ndroga1 1 droga2 2 droga3 3\n\nPeru\nPaco 1 Paco2 2 Pacojerlo 4\n\n");
	cstring level = cstring_copy("2\n\nAndorra\nPenicilina 300\nFaso 20\n\nPeru\nCaramelos 2000\n\nAndorra Peru 30\n\n");
	//deserialize them
	sim_airline a = sim_airline_deserialize(airline, -1);
	sim_level l = sim_level_deserialize(level);
	
	//serialize them
	cstring s2 = sim_airline_serialize(a, FALSE);
	cstring s3 = sim_level_serialize(l);
	
	l = sim_level_deserialize(s3);
	cstring s4 = sim_level_serialize(l);
	//print them
	//printf("%s\n%s\n%s\n",s1,s2,s3);
	assert(!cstring_compare(airline, s2));
	assert(!cstring_compare(s3, s4));
	printf("DONE!\n");
}

int main(int argc, char ** params) {
	
	printf("NONE OF THE FOLLOWING TESTS ARE GUARANTEED TO PASS ON ALL PLATFORMS\n");
	printf("THEY ARE USED AS ANOTHER DEVELOPMENT TOOL AND THEY ***ARE NOT*** MEANT\n");
	printf("TO BE EFFICIENT OR STABLE. THEY ARE MEANT TO GUARANTEE THAT THE TP CAN WORK\n");
	
//	file_test();
//	serializing_test();

//	message_counter = 0;
//	separator();
//	printf("STARTING FIFOS TEST\n");
//	separator();
//	networking_test(C_PIPE);
//
//	message_counter = 0;
//	separator();
//	printf("STARTING MESSAGE QUEUES TEST\n");
//	separator();
//	networking_test(C_M_QUEUES);
//	
//	message_counter = 0;
//	separator();
//	printf("STARTING SOCKETS TEST\n");
//	separator();
//	networking_test(C_SOCKETS);

	message_counter = 0;
	separator();
	printf("STARTING SHARED MEMORY TEST\n");
	separator();
	networking_test(C_SHARED_MEMORY);

	
	
	
	return 0;
}
