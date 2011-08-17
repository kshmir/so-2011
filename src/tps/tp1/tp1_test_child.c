#include <stdio.h>
#include <assert.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"


void networking_test(connection_type conn, int from_id, int to_id) {
	sim_transporter t = sim_transporter_init(conn, from_id, to_id);
	int i = 0;

	
	printf("Client sending data\n");
	for (; i < 200; i++) {
		sim_transporter_write(t,"cliente");
	}
	cstring data = sim_transporter_listen(t);
	
	assert(cstring_compare(data,"server") == 0);
	printf("Client got the correct information");
	
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