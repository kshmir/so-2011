/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------------------------------------------------------------
 * sim_server acts as a one-to-many information distribuiter.
 * It binds string sequences to functions which process the data.
 * It can also send a query message to it's clients to make a specific petition.
 */


#include "sim_server.h"
#include "sim_transporter.h"
#include <pthread.h>


// This is as AGLY as it lucs. 
// But it's the only way to override our vision of transporter over sockets.
extern int					sck_override;
extern queue				sck_override_queue;
extern pthread_mutex_t		sck_override_mutex;
extern pthread_cond_t		sck_override_received;

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
	
	/**
	 Stores the server id.
	 */
	int					server_id;
	
	/**
	 Used as a seed for the client.
	 */
	int					client_id_seed;
	
	/**
	 Uses a multiplier for the clientid.
	 */
	int					client_id_multiplier;
	
	int					spawn_sem;
	
	pthread_mutex_t * mutex		    ;
	pthread_cond_t	* listener_freed;
};


/**
 Cleans the server and all it's resources.
 */
static void sim_server_listener_cleanup(sim_server s) {
	
	pthread_cond_t * freed = s->listener_freed;
	map_free(s->responds_to);
	if (s->listen_transporter != NULL) {
		sim_transporter_free(s->listen_transporter);
	}

	
	list keys = map_keys(s->clients_transporters);
	int i = 0;
	for(; list_get(keys,i); i++) {
		sim_transporter_free(map_get(s->clients_transporters, list_get(keys,i)));
	}
	map_free(s->clients_transporters);
	list_free_with_data(keys);
	pthread_cond_signal(freed);	
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
	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	
	
	if (s->c_type == C_SOCKETS) {
		sck_override_queue = queue_init();
		pthread_mutex_init(&sck_override_mutex, NULL);
		pthread_cond_init(&sck_override_received, NULL);
		sck_override = 1;
		s->listen_transporter = NULL;
	}
	else
	{
		s->listen_transporter = sim_transporter_init(s->c_type, 
													 s->p_type, 
													 s->server_id, 
													 255, 
													 MODE_READ, 
													 FALSE, TRUE);
	}
	cstring last_msg = NULL;	
	while(TRUE) {

		cstring msg = NULL;
		
		if (s->c_type == C_SOCKETS) {
			while (queue_size(sck_override_queue) == 0) {
				
				pthread_cond_wait(&sck_override_received, &sck_override_mutex);
			}
			pthread_cond_broadcast(&sck_override_received);

			msg = queue_pull(sck_override_queue);
		} else {
			msg = sim_transporter_listen(s->listen_transporter, last_msg);
		}
		
		
		cstring header = cstring_copy_until_char(msg, ';');
		
		pthread_testcancel();
		
		
		int fail = 1;		

		foreach(cstring, key, s->responds_to_keys) {
			cstring safe_key = cstring_copy(key);
			if (cstring_matches(header, safe_key) == 1 || cstring_compare(safe_key,header) == 0) {
				list params = cstring_split_list(msg, ";");
				list header_values = cstring_split_list((cstring)list_get(params,0)," ");
				int ok = 1;
				int id = cstring_parseInt((cstring)list_get(header_values,0), &ok);
				
				sim_transporter t = NULL ;
				
				while(t == NULL) {
					t = (sim_transporter)map_get(s->clients_transporters,&id);
				}
				
				cstring _msg = cstring_copy("RES ");
				_msg = cstring_write(_msg, list_get(params,0));
				sim_message _m = sim_message_init(t, _msg, cstring_copy(list_get(params,1)));
				((function)map_get(s->responds_to, safe_key))(_m);
				
				list_free_with_data(params);
				list_free_with_data(header_values);
				sim_message_free(_m);
				if (last_msg != NULL) {
					free(last_msg);
				}
				last_msg = NULL;
				fail = 0;
				if (s->c_type != C_SOCKETS) {
					sim_transporter_dequeue(s->listen_transporter);
				} else {
					free(msg);
				}

				free(safe_key);
				break;
			} else {
				if (s->c_type != C_SOCKETS) {
					last_msg = msg;
				}

			}

			free(safe_key);
		}
		free(header);
	}
	
	pthread_cleanup_pop(0);
}

/**
 Starts the server and it's local listener.
 */
sim_server sim_server_init(connection_type con, process_type p_type, int server_id) {		
	sim_server s = (sim_server) malloc(sizeof(struct sim_server));


	s->responds_to = map_init(cstring_comparer, NULL);
	s->responds_to_keys = list_init();
	s->clients_transporters = map_init(int_comparer, int_cloner);
	s->c_type = con;
	s->p_type = p_type;
	
	s->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	s->listener_freed = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	
	pthread_mutex_init(s->mutex, NULL);
	pthread_cond_init(s->listener_freed, NULL);

	
	s->spawn_sem = sem_create_typed("spawn");
	s->server_id = server_id;


	
	switch (p_type) {
		case P_LEVEL:
			s->client_id_seed = 5;
			s->client_id_multiplier = 1;
			break;
		case P_AIRLINE:
			s->client_id_seed = 100;
			s->client_id_multiplier = 1;
			break;
		default:
			s->client_id_seed = 10;
			s->client_id_multiplier = 1;
	}
	
	
	pthread_create(&s->listener_thread, NULL, (void_p) sim_server_listener, (void_p) s);	
	return s;
}


/**
 Sends a broadcast query to it's clients.
 It's used for clients responding to actions like stop or tick.
 */
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

/**
 Binds a char sequence to a receiver, it allows the server to responds to events.
 */
int sim_server_add_receiver(sim_server s, cstring sequence, sim_receiver rec) {
	map_set(s->responds_to, sequence, rec);
	list_add(s->responds_to_keys, sequence);
}

/**
 Creates a new client process and makes a transporter which can write to it.
 */
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
	sem_down(s->spawn_sem, 1);

	
//	cprintf("I spawn my child %d\n", OK, key);
	

	
	s->client_id_multiplier++;

}

/**
 Free's the server and it's thread.
 */
int sim_server_free(sim_server s) {
	pthread_cond_t * freed = s->listener_freed;
	pthread_mutex_t * mutex = s->mutex;
	//sem_free_typed(s->spawn_sem, "spawn");
	pthread_cancel(s->listener_thread);
	
	struct timespec {
		long ts_sec; /* seconds */
		long ts_nsec; /* nanoseconds */
	} to;
	
	to.ts_sec = time(NULL) + 1;
	to.ts_nsec = 0;
	pthread_cond_timedwait(freed, mutex, (void_p) &to);
	pthread_cond_destroy(freed);
	pthread_mutex_destroy(mutex);
}