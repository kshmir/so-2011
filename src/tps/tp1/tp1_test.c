#include <stdio.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"


void core_test() {
	
	
	
}

void networking_test() {
	sim_transporter t = sim_transporter_fork(C_PIPE, P_TESTER, 0, 1);
	cstring ans = sim_transporter_listen(t);
	printf("%s", ans);
}

int main(int argc, char ** params) {
	
	networking_test();
	core_test();
	
	printf("I get started!!! :D");
	return 0;
}