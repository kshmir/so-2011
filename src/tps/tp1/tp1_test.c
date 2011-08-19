#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "networking/sim_message.h"
#include "networking/sim_server.h"
#include "networking/sim_transporter.h"
#include "sim_common_reqs.h"

void separator() {
	printf("--------------------------------\n");
}

void core_test() {
	
	
	
}



void file_test(){
	printf("%s", cstring_from_file("./testFile"));
}

cstring buffer;

void receiver(sim_message mes) {
	buffer = cstring_write(buffer,sim_message_read(mes));
	sim_message_free(mes);
}

void networking_test() {
	separator();
	
	sim_transporter t = sim_transporter_init(C_PIPE, P_TESTER, 0, 1, MODE_READWRITE, TRUE, TRUE);

	sim_transporter_write(t, "server");
	cstring data = sim_transporter_listen(t);
	sim_transporter_dequeue(t);
	
	separator();
	
	sim_message m = sim_message_init(t, "POST", "Write again");
	sim_message resp = sim_message_send(m);
	data = sim_message_read(resp);
	assert(cstring_compare(data, "Hello Baby"));
	
	sim_client c = sim_client_init(t, receiver);
	
}

void serializing_test(){
	
	//create the strings to be tested
	printf("Serializing & deserializing planes:\n");
	cstring plane = cstring_copy("Andorra\ndroga1 1 droga2 2 droga3 3\n");
	cstring airline = cstring_copy("2\n\nAndorra\ndroga1 1 droga2 2 droga3 3\n\nPeru\nPaco 1 Paco2 2 Pacojerlo 4\n\n");
	cstring level = cstring_copy("2\n\nAndorra\nPenicilina 300\nFaso 20\n\nPeru\nCaramelos 2000\n\nAndorra Peru 30\n\n");
	//deserialize them
	sim_plane p = sim_plane_deserialize(plane, -1);
	sim_airline a = sim_airline_deserialize(airline, -1);
	sim_level l = sim_level_deserialize(level);
	
	//serialize them
	cstring s1 = sim_plane_serialize(p);
	cstring s2 = sim_airline_serialize(a, FALSE);
	cstring s3 = sim_level_serialize(l);
	
	l = sim_level_deserialize(s3);
	cstring s4 = sim_level_serialize(l);
	//print them
	//printf("%s\n%s\n%s\n",s1,s2,s3);
	assert(!cstring_compare(plane, s1));
	assert(!cstring_compare(airline, s2));
	assert(!cstring_compare(s3, s4));
	printf("DONE!\n");
}

int main(int argc, char ** params) {
	
	buffer = cstring_init(0);
	//file_test();
	//serializing_test();
	
	networking_test();
	return 0;
}
