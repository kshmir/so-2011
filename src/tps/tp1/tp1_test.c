#include <stdio.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"
#include "sim_common_reqs.h"

void core_test() {
	
	
	
}

void file_test(){
	printf("%s/n", cstring_from_file("testFile"));
}

void networking_test() {
	sim_transporter t = sim_transporter_fork(C_PIPE, P_TESTER, 0, 1);
	sleep(1);
	sim_transporter_write(t, "WOW\n");
	cstring data = sim_transporter_listen(t);
	printf("Client: %s", data);
}

int main(int argc, char ** params) {
	
	file_test();
	//networking_test();
	return 0;
}