#include <stdio.h>
#include <assert.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"


void separator() {
	printf("--------------------------------\n");
}

void networking_test(connection_type conn, int from_id, int to_id) {
	sim_transporter t = sim_transporter_init(conn, P_TESTER, from_id, to_id, MODE_READWRITE, FALSE, FALSE);
	

	int i = 0;
	
	
	// Starts transporter test
	
	sim_transporter_write(t,"cliente");
	
	cstring data = sim_transporter_listen(t);
	sim_transporter_dequeue(t);
	
	assert(cstring_compare(data,"server") == 0);
	
	// Ends transporter test
	
	
	// Starts response of message test
	
	data = sim_transporter_listen(t);
	assert(cstring_compare("POST;Write again", data));
	
	//////////// This should be a server work
	
	sim_transporter_write(t, "RES POST;Hello baby"); 
	
	sim_transporter_write(t, "QUERY ;asdkalsdjasd"); 
	sim_transporter_write(t, "QUERY ;asdkalsdjasd2"); 
	sim_transporter_write(t, "QUERY ;asdkalsdjasd3"); 
	sim_transporter_write(t, "QUERY ;asdkalsdjasd4"); 
	
	sim_server serv = sim_server_init(t);
	
	
	
	
	// End resposne of message test
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