/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------------------------------------------------------------
 * sim_client is used as a way to communicate with it's owner, aka server.
 * It has several handlers which lock until they receive a response from the server, which can be
 * specific to the game, such as filling a city with medicine or sending a response at the end of a turn.
 * It also allows an abstraction layer from the transporter.
 * It stands over message and transporter.
 */
#include "sim_client.h"

#include <pthread.h>
#include <time.h>

/**
 * Data structure for the client.   
 */
struct sim_client {
	
	sim_transporter t;						// Internal transporter used by the client.
	sim_receiver	r;						// Query receiver.
	
	int	spawn_sem;
	
	int client_id;							// ID to which it listens.
	
	pthread_t		listener_thread;		// Listener thread.
	pthread_mutex_t * mutex		    ;
	pthread_cond_t	* listener_freed;
};

/**
 * Cleans the client after a callback which stops the thread.
 */
static void sim_client_listener_cleanup(sim_client r) {
	pthread_cond_t * freed = r->listener_freed;
	sim_transporter_free(r->t);
	pthread_cond_signal(freed);
	free(r);

}

/**
 * Listener for query petitions.
 */
static void_p sim_client_listener(sim_client r) {
	int oldtype;
	pthread_cleanup_push((void_p)sim_client_listener_cleanup, r);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
	
	cstring last_msg = NULL;
	while(TRUE) {
		cstring msg = sim_transporter_listen(r->t, last_msg);

		pthread_testcancel();
		


		cstring header = cstring_copy_until_char(msg, ';');
		
		if (cstring_compare(header,"QUERY ") == 0) {

			cstring no_resp = cstring_replace(msg, "QUERY ", "");
			list splitted = cstring_split_list(no_resp, ";");

	
			if (list_size(splitted) == 2) {
				sim_message m = sim_message_init(r->t,list_get(splitted, 0), list_get(splitted, 1));
				r->r(m);
				sim_transporter_dequeue(r->t);
				sim_message_free(m);
			}
			
			if (last_msg != NULL) {
				free(last_msg);
			}
			last_msg = NULL;
			list_free(splitted);
			free(no_resp);
		} else {
			if (last_msg != NULL) {
				free(last_msg);
			}
			last_msg = msg;
		}

		free(header);
	}

	pthread_cleanup_pop(0);
}

/**
 * Starts the client from an existing transporter, it allows to make the construction more flexible. for example, for testing.
 */
sim_client sim_client_from_transporter(sim_transporter t, sim_receiver r) {
	sim_client c = (sim_client) malloc(sizeof(struct sim_client));
	c->t = t;
	c->r = r;
	c->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	c->listener_freed = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));

	pthread_mutex_init(c->mutex, NULL);
	pthread_cond_init(c->listener_freed, NULL);
	
	c->spawn_sem = sem_create_valued(1244422, 1);

	pthread_create(&c->listener_thread, NULL, (void_p) sim_client_listener, (void_p) c);	
//	sem_up(c->spawn_sem, 1);
	return c;
}

/**
 * Starts the client with the parameters received, usually used after being fork'd/exec'd by a server.
 */
sim_client sim_client_init(connection_type c_type, process_type p_type, int server_id, int client_id, sim_receiver query_receiver) {
	sim_transporter t = sim_transporter_init(c_type, p_type, server_id, client_id, MODE_READWRITE, FALSE, FALSE);
	return sim_client_from_transporter(t, query_receiver);
}

/**
 * Gets a distance between points, we should check if we must really use this or not.
 */
int sim_client_get_distance(sim_client c, int object_id, cstring from, cstring to) {
	cstring header = cstring_fromInt(object_id); 
	header = cstring_write(header,cstring_copy(" DIST"));
	cstring get = cstring_copy(from);
	get = cstring_write(get, " ");
	get = cstring_write(get, to);
	sim_message request = sim_message_init(c->t, header, get);
	sim_message response = sim_message_send(request);
	// Rebuild response
	// Response should be... RES {object_id} DIST {from} {to};INT_VALUE
	cstring resp = sim_message_read(response);
	list params = cstring_split_list(resp, ";");
	int code = 0;
	int value = cstring_parseInt(list_get(params, 1), &code);
	sim_message_free(response);
	list_free_with_data(params);
	return value;
}

/**
 * Gets all the medicines from a point.
 */
void_p sim_client_copy_level(sim_client c, int object_id) {
	cstring header = cstring_fromInt(object_id); 
	header = cstring_write(header, cstring_copy(" COPY_LEVEL"));
	cstring get = cstring_copy("");
	sim_message request = sim_message_init(c->t, header, get);
	sim_message response = sim_message_send(request);

	cstring mes = cstring_copy(sim_message_read(response));
	sim_message_free(response);
	return sim_level_deserialize(mes);
}

list sim_client_copy_airline(sim_client c, int object_id) {
	cstring header = cstring_fromInt(object_id); 
	header = cstring_write(header, cstring_copy(" COPY_AIR"));
	list l = list_init();
	int i = 0;
	int end = 0;
	while (!end) {
		cstring _get = cstring_fromInt(i);
		cstring _header = cstring_copy(header);
		sim_message request = sim_message_init(c->t, _header, _get);
		sim_message response = sim_message_send(request);
		cstring data = sim_message_read(response);
		if (cstring_compare(data, "null") == 0) {
			end = TRUE;
		} else {
			sim_airline air = sim_airline_deserialize(data, -1);
			list_add(l, air);
		}
		sim_message_free(response);
		i++;
	}
	return l;
}

void_p sim_client_copy_single_airline(sim_client c, int object_id) {

	cstring header = cstring_fromInt(object_id); 
	header = cstring_write(header, cstring_copy(" COPY_SAIR"));

	cstring get = cstring_fromInt(object_id);

	sim_message request = sim_message_init(c->t, header, get);
	sim_message response = sim_message_send(request);
	cstring data = sim_message_read(response);
	

	sim_airline air = sim_airline_deserialize(data, object_id);

	sim_message_free(response);
	return air;
}


/**
 * Fills a point with medicine.
 */
int sim_client_post_medicine_fill(sim_client c, int object_id, cstring city, cstring medicine, int plane_id, int amount) {
	cstring header = cstring_fromInt(object_id); 
	cstring id = cstring_fromInt(plane_id);
	header = cstring_write(header, cstring_copy(" MEDF "));
	header = cstring_write(header, id);
	cstring get = cstring_copy(city);
	cstring am = cstring_fromInt(amount);

	get = cstring_write(get, " ");
	get = cstring_write(get, medicine);	
	get = cstring_write(get, " ");
	get = cstring_write(get, am);
	get = cstring_write(get, " ");
	get = cstring_write(get, id);
	sim_message request = sim_message_init(c->t, header, get);
	sim_message response = sim_message_send(request);
	
	cstring rsp = sim_message_read(response);
//	cprintf("MEDICINE FILL: %s\n", ROJO, rsp);
	int noerror = 0;
	int val = cstring_parseInt(rsp, &noerror);
	sim_message_free(response);
	free(id);
	free(am);
	// Rebuild response
	// Response should be... RES {object_id} MEDF;{value}
	// Where value is -1 if there's an error, or the remaining amount of medicine.
//	cprintf("MEDICINE FILL INT: %d\n", ROJO, val);
	return val;
}

/**
 * Makes a move from a plane to another point
 */
int sim_client_post_plane_move(sim_client c, int object_id, int plane_id, cstring city_from, cstring city_to) {
	cstring header = cstring_fromInt(object_id); 
	header = cstring_write(header, cstring_copy(" PMOV"));
	cstring get = cstring_fromInt(plane_id);
	get = cstring_write(get, " ");
	get = cstring_write(get, city_from);	
	get = cstring_write(get, " ");
	get = cstring_write(get, city_to);
	sim_message request = sim_message_init(c->t, header, get);
	sim_message response = sim_message_send(request);
	cstring rsp = sim_message_read(response);
	int noerror = 0;
	int val = cstring_parseInt(rsp, &noerror);
	sim_message_free(response);
	// Rebuild response
	// Response should be... RES {object_id} MEDF;{value}
	// Where value is -1 if there's an error, or the remaining amount of medicine.
	return val;
}

/**
 * Sends a print message to a server.
 */
int sim_client_print(sim_client c, cstring message, int _id) {
	
	int id = (_id != -1) ? _id : c->client_id;
	
	cstring header = cstring_write(cstring_fromInt(id), " PRINT ");
	sim_message request = sim_message_init(c->t, header, message);
	sim_message_respond(request);
	sim_message_free(request);
	return 1;
}

/**
 * Cleans the client, it's thread and it's data.
 */
void sim_client_free(sim_client c) {
	pthread_cond_t * freed = c->listener_freed;
	pthread_mutex_t * mutex = c->mutex;
	
	sem_free_typed(c->spawn_sem, "spawn");
	pthread_cancel(c->listener_thread);
	
	struct timespec {
		long ts_sec; /* seconds */
		long ts_nsec; /* nanoseconds */
	} to;
	
	to.ts_sec = time(NULL);
	to.ts_nsec = 1000 * 1000 * 200;
	pthread_cond_timedwait(freed, mutex, (void_p)&to);
	pthread_cond_destroy(freed);
	pthread_mutex_destroy(mutex);
}






