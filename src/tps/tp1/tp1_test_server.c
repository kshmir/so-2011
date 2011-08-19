#include <stdio.h>
#include <assert.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"


void separator() {
	printf("--------------------------------\n");
}

void networking_test(connection_type conn, int from_id, int to_id) {
	// Starts a client and makes a lot of noise.
	

	// Stops sending noise to the server.
}

int main(int argc, char ** params) {
	if (argc == 4) {	
		int parsed_1 = 0;
		int connection_t = cstring_parseInt(params[1], &parsed_1);
		int parsed_2 = 0;
		int from_id = cstring_parseInt(params[2], &parsed_2);
		int parsed_3 = 0;
		int to_id = cstring_parseInt(params[3], &parsed_3);
		if (parsed_1 && parsed_2 && parsed_3) {
			networking_test(connection_t, from_id, to_id);
		}
	}
	return 0;
}