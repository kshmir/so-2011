#include <stdio.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"


void core_test() {
	
	
	
}

void networking_test() {
	sim_transporter t = sim_transporter_fork(C_PIPE, P_TESTER, 0, 1);
	sleep(1);
	sim_transporter_write(t, "WOW\n");
	cstring data = sim_transporter_listen(t);
	printf("Client: %s", data);
}

int main(int argc, char ** params) {
	
	networking_test();
	return 0;
}