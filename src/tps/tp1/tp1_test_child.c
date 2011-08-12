#include <stdio.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"


void networking_test(connection_type n, process_type p) {
	sim_transporter t = sim_transporter_fork(n, p, 1, 0);
	sim_transporter_write(t, "Hola");
	printf("%s", ans);
}

int main(int argc, char ** params) {
	if (argc == 3) {	
		int parsed_1 = 0;
		int connection_t = cstring_parseInt(params[1], &parsed_1);
		int parsed_2 = 0;
		int process_t = cstring_parseInt(params[2], &parsed_2);
		if (parsed_1 && parsed_2) {
			networking_test(connection_t, process_t);
		}
	}
	return 0;
}