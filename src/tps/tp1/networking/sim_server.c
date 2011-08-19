#include "sim_server.h"
#include "sim_transporter.h"
#include <pthread.h>

/**
	Handles all the connections across clients transparently.
	It first starts listening on a thread with a read-only sim_transporter.
	When it receives a matching key, it sends the associated receiver to it.
*/
struct sim_server {
	/**
		Stores as keys the sequence to match.
		Stores as values the receiver function to invoke.
	 */
	map					responds_to;
	/** 
		List of the params stored as keys to respond for each value.
	 */
	list				responds_to_keys;

	/**
		Stores the kind of connection to use between clients and server.
	 */
	connection_type		c_type;
	
	
	/**
		Stores the kind of process to open as a child
	 */
	process_type		p_type;


	/**
		The thread used to listen to clients.
	 */
	pthread_t			listener_thread;
	
	/**
		The read only transporter used to listen to clients.
	 */
	sim_transporter		listen_transporter;

	/**
		Stores integers (IDs) of each client as keys.
		Stores transporters   of each client as values.
	 */
	map					clients_transporters;
	
	int					server_id;
	
	int					client_id_seed;
	
	int					client_id_multiplier;
};

/**
 Listens to all the messages being sent to the server.
 When matches a key string, it makes a response.
 Then keeps on listening.
 */
static void sim_server_listen(sim_server s) {
	return;
	while(TRUE) {
		cstring msg = sim_transporter_listen(s->listen_transporter);
		cstring header = cstring_copy_until_char(msg, ';');
		
		
		foreach(cstring, key, s->responds_to_keys) {
			
		}
	}
}

sim_server sim_server_init(connection_type con, process_type p_type, int server_id) {		
	sim_server s = (sim_server) malloc(sizeof(struct sim_server));
	s->responds_to = map_init(cstring_comparer, cstring_cloner);
	s->responds_to_keys = list_init();
	s->clients_transporters = map_init(int_comparer, int_cloner);
	s->c_type = con;
	s->p_type = p_type;
	s->client_id_seed = 1;
	s->server_id = server_id;
	s->listen_transporter = sim_transporter_init(con, p_type, server_id, server_id, MODE_READ, FALSE, TRUE);
	
	switch (p_type) {
		case P_LEVEL:
			s->client_id_multiplier = 1;
			break;
		case P_AIRLINE:
			s->client_id_multiplier = 1000;
			break;
		default:
			s->client_id_multiplier = 1000;
	}
	
	
	pthread_create(&s->listener_thread, NULL, (void_p) sim_server_listen, (void_p) s);	
	return s;
}



int sim_server_add_receiver(sim_server s, cstring sequence, sim_receiver rec) {
	map_set(s->responds_to, sequence, rec);
	list_add(s->responds_to_keys, sequence);
}

int sim_server_spawn_child(sim_server s) {
	
	sim_transporter child_t = sim_transporter_init(s->c_type,
												   s->p_type, 
												   s->server_id, 
												   s->client_id_seed * s->client_id_multiplier, 
												   MODE_WRITE, 
												   TRUE, 
												   TRUE);
	int key = s->client_id_seed * s->client_id_multiplier;
	
	map_set(s->clients_transporters, &key, child_t);
	
	s->client_id_seed++;
}