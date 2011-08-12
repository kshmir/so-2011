#include "sim_server.h"
#include "sim_transporter.h"

struct sim_server {
	// Stores the keys it responds to and the pointers to functions to receive a callback
	map responds_to;

	connection_type c_type;
	
	// Handles all the messaging across clients.
	map transporters;
};

sim_server sim_server_init(connection_type con) {		
	sim_server s = (sim_server) malloc(sizeof(struct sim_server));
	s->responds_to = map_init(cstring_comparer, cstring_cloner);
	s->transporters = map_init(pointer_comparer, NULL);
	s->c_type = con;
	return s;
}


void sim_server_listen() {
	
}

int sim_server_add_receiver(sim_server s, cstring sequence, sim_receiver function) {
	return map_set(s->responds_to, sequence, function);
}