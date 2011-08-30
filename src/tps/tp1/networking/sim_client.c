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

/**
 * Data structure for the client.   
 */
struct sim_client {
	
	sim_transporter t;						// Internal transporter used by the client.
	sim_receiver	r;						// Query receiver.
	
	int	spawn_sem;
	
	int client_id;							// ID to which it listens.
	
	pthread_t		listener_thread;		// Listener thread.
};

/**
 * Cleans the client after a callback which stops the thread.
 */
static void sim_client_listener_cleanup(sim_client r) {
	sim_transporter_free(r->t);
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
	
	while(TRUE) {
		cstring msg = sim_transporter_listen(r->t);
		
		if (msg[0] == 0) {
			return NULL; 
		}


		cstring header = cstring_copy_until_char(msg, ';');
		
		if (cstring_compare(header,"QUERY ") == 0) {
			cstring no_resp = cstring_replace(msg, "QUERY ", "");
			list splitted = cstring_split_list(no_resp, ";");

			if (list_size(splitted) == 2) {
				r->r(sim_message_init(r->t,list_get(splitted, 0), list_get(splitted, 1)));
				sim_transporter_dequeue(r->t);
			}
			
			list_free(splitted);
			free(no_resp);
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
	c->spawn_sem = sem_create_typed(sim_transporter_client_id(t), "spawn");
	pthread_create(&c->listener_thread, NULL, (void_p) sim_client_listener, (void_p) c);	
	sem_up(c->spawn_sem, 1);
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
	sim_message_free(request);
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
sim_level sim_client_copy_level(sim_client c, int object_id) {
	cstring header = cstring_fromInt(object_id); 
	header = cstring_write(header, cstring_copy(" COPY_LEVEL"));
	cstring get = cstring_copy("");
	sim_message request = sim_message_init(c->t, header, get);
	sim_message response = sim_message_send(request);

	return sim_level_deserialize(sim_message_read(response));
}

list sim_client_copy_airline(sim_client c, int object_id) {
	cstring header = cstring_fromInt(object_id); 
	header = cstring_write(header, cstring_copy(" COPY_AIR"));
	list l = list_init();
	int i = 0;
	int end = 0;
	while (!end) {
		cstring get = cstring_fromInt(i);
		sim_message request = sim_message_init(c->t, header, get);
		sim_message response = sim_message_send(request);
		cstring data = sim_message_read(response);
		if (cstring_compare(data, "null") == 0) {
			end = TRUE;
		} else {
			sim_airline air = sim_airline_deserialize(data, -1);
			list_add(l, air);
		}
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

	printf("GOT %d planes and id %d\n", list_size(sim_airline_planes(air)), sim_airline_id(air));
	free(data);
	return air;
}


/**
 * Fills a point with medicine.
 */
int sim_client_post_medicine_fill(sim_client c, int object_id, cstring city, cstring medicine, int amount) {
	cstring header = cstring_fromInt(object_id); 
	header = cstring_write(header, cstring_copy(" MEDF"));
	cstring get = cstring_copy(city);
	get = cstring_write(get, " ");
	get = cstring_write(get, medicine);	
	get = cstring_write(get, " ");
	get = cstring_write(get, cstring_fromInt(amount));
	sim_message request = sim_message_init(c->t, header, get);
	sim_message response = sim_message_send(request);
	sim_message_free(request);
	// Rebuild response
	// Response should be... RES {object_id} MEDF;{city} {medicine} {value}
	// Where value is -1 if there's an error, or the remaining amount of medicine.
	return 0;
}

/**
 * Sends a print message to a server.
 */
int sim_client_print(sim_client c, cstring message, int _id) {
	
	int id = (_id != -1) ? _id : c->client_id;
	
	cstring header = cstring_write(cstring_fromInt(id), " PRINT ");
	sim_message request = sim_message_init(c->t, header, message);
	sim_message_respond(request);
	free(header);
	return 1;
}

/**
 * Cleans the client, it's thread and it's data.
 */
void sim_client_free(sim_client c) {
	pthread_cancel(c->listener_thread);
}






