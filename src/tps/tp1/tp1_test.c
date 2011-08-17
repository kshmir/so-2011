#include <stdio.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"
#include "sim_common_reqs.h"

void core_test() {
	
	
	
}

void file_test(){
	printf("%s", cstring_from_file("./testFile"));
}

void networking_test() {
	sim_transporter t = sim_transporter_fork(C_PIPE, P_TESTER, 0, 1);
	sleep(1);
	sim_transporter_write(t, "WOW\n");
	cstring data = sim_transporter_listen(t);
	printf("Client: %s", data);
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
	
	//file_test();
	serializing_test();
	//networking_test();
	return 0;
}
