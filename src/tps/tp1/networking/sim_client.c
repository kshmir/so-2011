#include "sim_client.h"

#include <pthread.h>

struct sim_client {
	sim_transporter t;
	sim_receiver	r;
	
	int client_id;
	
	pthread_t		listener_thread;
};

static void sim_client_listener_cleanup(sim_client r) {
	sim_transporter_free(r->t);
	//free(r);
}

static void_p sim_client_listener(sim_client r) {
	int oldtype;
	pthread_cleanup_push((void_p)sim_client_listener_cleanup, r);
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



sim_client sim_client_from_transporter(sim_transporter t, sim_receiver r) {
	sim_client c = (sim_client) malloc(sizeof(struct sim_client));
	c->t = t;
	c->r = r;
	pthread_create(&c->listener_thread, NULL, (void_p) sim_client_listener, (void_p) c);	
	return c;
}

sim_client sim_client_init(connection_type c_type, process_type p_type, int server_id, int client_id, sim_receiver query_receiver) {
	sim_transporter t = sim_transporter_init(c_type, p_type, server_id, client_id, MODE_READWRITE, FALSE, FALSE);
	return sim_client_from_transporter(t, query_receiver);
}

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

map sim_client_get_medicines(sim_client c, int object_id, cstring from) {
	cstring header = cstring_fromInt(object_id); 
	header = cstring_write(header, cstring_copy(" MEDS"));
	cstring get = cstring_copy(from);
	sim_message request = sim_message_init(c->t, header, get);
	sim_message response = sim_message_send(request);
	sim_message_free(request);
	// Rebuild map from response
	// Response should be... RES {object_id} MEDS;(list of medicines as saved in files)
	
	return NULL;
}


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

int sim_client_print(sim_client c, cstring message, int _id) {
	
	int id = (_id != -1) ? _id : c->client_id;
	
	cstring header = cstring_write(cstring_fromInt(id), " PRINT ");
	sim_message request = sim_message_init(c->t, header, message);
	sim_message_respond(request);
	free(header);
	return 1;
}

void sim_client_free(sim_client c) {
	pthread_cancel(c->listener_thread);
}






