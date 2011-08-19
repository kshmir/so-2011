#include "sim_server.h"
#include "sim_transporter.h"
#include <pthread.h>

struct sim_server {
	// Stores the keys it responds to and the pointers to functions to receive a callback
	map					responds_to;
	list				responds_to_keys;

	connection_type		c_type;
	
	pthread_t			listener_thread;
	
	sim_transporter		listen_transporter;

	
	// Handles all the messaging across clients.
	map transporters;
};

sim_server sim_server_init(connection_type con) {		
	sim_server s = (sim_server) malloc(sizeof(struct sim_server));
	s->responds_to = map_init(cstring_comparer, cstring_cloner);
	s->transporters = map_init(pointer_comparer, NULL);
	s->c_type = con;
	
	pthread_create(&s->listener_thread, NULL, (void_p) sim_server_listen, (void_p) s);	
	return s;
}


void sim_server_listen(sim_server s) {
	while(TRUE) {
		cstring msg = sim_transporter_listen(s->listen_transporter);
		cstring header = cstring_copy_until_char(msg, ';');
		
		foreach(cstring, key, s->responds_to_keys) {
			
		}
	}
}

int sim_server_add_receiver(sim_server s, cstring sequence, sim_receiver function) {
	return map_set(s->responds_to, sequence, function);
}