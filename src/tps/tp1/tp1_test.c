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
	sim_transporter_write(t, "MAXI GAY\n");
}

int main(int argc, char ** params) {
	
	file_test();
	//networking_test();
	return 0;
}