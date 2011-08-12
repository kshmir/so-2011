#include <stdio.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"


void core_test() {
	
	
	
}

void networking_test() {
	sim_transporter t = sim_transporter_fork(C_PIPE, P_TESTER, 0, 1);
	sim_transporter_write(t, "MAXI GAY\n");
	printf("end server");
}

int main(int argc, char ** params) {
	
	networking_test();
	return 0;
}