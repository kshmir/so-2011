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
	printf("String:\n");
	cstring plane = cstring_copy("Andorra\ndroga1 1\ndroga2 2\ndroga3 3\n");
	printf("%s",plane);
	//deserialize them
	printf("Deserialized:\n");
	sim_plane p = sim_plane_deserialize(plane, 0);
	//print structure
	sim_plane_print(p);
	//serialize them
	printf("Serialized:\n");
	cstring s = sim_plane_serialize(p);
	//print them
	printf("%s",s);
}

int main(int argc, char ** params) {
	
	buffer = cstring_init(0);
	//file_test();
	//serializing_test();
	
	networking_test();
	return 0;
}
