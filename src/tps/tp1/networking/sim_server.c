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


static void sim_server_listener_cleanup(sim_server s) {
	map_free(s->responds_to);
	sim_transporter_free(s->listen_transporter);
	
	list keys = map_keys(s->clients_transporters);
	int i = 0;
	for(; list_get(keys,i); i++) {
		sim_transporter_free(map_get(s->clients_transporters, list_get(keys,i)));
	}
	map_free(s->clients_transporters);
	list_free_with_data(keys);
	
	free(s);
}

/**
 Listens to all the messages being sent to the server.
 When matches a key string, it makes a response.
 Then keeps on listening.
 */
static void sim_server_listener(sim_server s) {

	
	int oldtype;
	pthread_cleanup_push((void_p)sim_server_listener_cleanup, s);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
	
	s->listen_transporter = sim_transporter_init(s->c_type, 
												 s->p_type, 
												 s->server_id, 
												 s->server_id, 
												 MODE_READ, 
												 FALSE, TRUE);
	int count = 0;
	while(TRUE) {
		cstring msg = sim_transporter_listen(s->listen_transporter);
		cstring header = cstring_copy_until_char(msg, ';');
		

		
		int fail = 1;		
		foreach(cstring, key, s->responds_to_keys) {
			//printf("hola: %s %d\n",msg, count++);
			cstring safe_key = cstring_copy(key);
			if (cstring_matches(header, safe_key) == 1 || cstring_compare(safe_key,header) == 0) {
				sim_transporter_dequeue(s->listen_transporter);
				list params = cstring_split_list(msg, ";");
				list header_values = cstring_split_list((cstring)list_get(params,0)," ");
				int ok = 1;
				int id = cstring_parseInt((cstring)list_get(header_values,0), &ok);
				sim_message _m = sim_message_init((sim_transporter)map_get(s->clients_transporters,&id), 
																   cstring_write(cstring_copy("RES "),list_get(params,0)), 
																   list_get(params,1));
				((function)map_get(s->responds_to, &safe_key))(_m);
				
				list_free_with_data(params);
				list_free_with_data(header_values);
				fail = 0;
			}
			free(safe_key);
		}
		if (fail == 1) {
			printf("msg: %s\n", msg);
		}
	}
	
	pthread_cleanup_pop(0);
}

sim_server sim_server_init(connection_type con, process_type p_type, int server_id) {		
	sim_server s = (sim_server) malloc(sizeof(struct sim_server));
	s->responds_to = map_init(cstring_comparer, NULL);
	s->responds_to_keys = list_init();
	s->clients_transporters = map_init(int_comparer, int_cloner);
	s->c_type = con;
	s->p_type = p_type;
	s->client_id_seed = 1;
	s->server_id = server_id;

	
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
	
	
	pthread_create(&s->listener_thread, NULL, (void_p) sim_server_listener, (void_p) s);	
	return s;
}


void sim_server_broadcast_query(sim_server s, cstring message) {
	cstring header = cstring_copy("QUERY ");
	cstring msg = cstring_copy(message);
	
	
	list transporters = map_values(s->clients_transporters);
	foreach(sim_transporter, t, transporters) {
		sim_message m = sim_message_init(t, cstring_copy(header), cstring_copy(msg));
		sim_message_respond(m);
		sim_message_free(m);
	}
	list_free(transporters);
	free(header);
	free(msg);
}

int sim_server_add_receiver(sim_server s, cstring sequence, sim_receiver rec) {
	int * ptr = (int *) malloc(sizeof(void_p));
	* ptr = (int) sequence;
	map_set(s->responds_to, ptr, rec);
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


int sim_server_free(sim_server s) {
	pthread_cancel(s->listener_thread);
}