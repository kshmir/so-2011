#include "../sim_common_reqs.h"
#include "sim_transporter.h"

struct sim_server {
	// Stores the keys it responds to and the pointers to functions to receive a callback
	map responds_to;

	connection_type c_type;
	
	// Handles all the messaging across clients.
	map transporters;
}

sim_server sim_server_init(connection_type con) {		
	sim_server s = (sim_server) malloc(sizeof(struct sim_server));
	s->responds_to = map_init(string_comparer, string_cloner);
	s->transporters = map_init(pointer_comparer, NULL);
	s->c_type = con;
	return s;
}

// Starts a new client, sending the process type as an arg and the connection type.
int	sim_server_fork(process_type proc) {
	// Opens a transporter between processes
	// Transporter handles the way to switch between processes.
	// Also opens a thread which listens to the transporter.
	// When it receives a message from the transporter with a valid message, it sends it to it's receiver.
}

int sim_server_add_receiver(sim_server s, cstring sequence, sim_receiver function) {
	return map_set(responds_to, sequence, function);
}