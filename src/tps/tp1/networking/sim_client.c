#include "sim_client.h"

#include <pthread.h>

struct sim_client {
	sim_transporter t;
	sim_receiver	r;
	
	pthread_t		listener_thread;
};

static void_p sim_client_listen(sim_client r) {
	while(TRUE) {
		cstring msg = sim_transporter_listen(r->t);
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
}







sim_client sim_client_init(sim_transporter t, sim_receiver r) {
	sim_client c = (sim_client) malloc(sizeof(sim_client));
	c->t = t;
	c->r = r;
	pthread_create(&c->listener_thread, NULL, (void_p) sim_client_listen, (void_p) c);	
	return c;
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

int sim_client_post_movement(sim_client c, int object_id, cstring from, cstring to) {
	//	cstring get = cstring_fromInt(object_id); 
	//	cstring_copy(" POST MOV ");
	//	cstring_write(get, " ");
	//	cstring_write(get, from);
	//	cstring_write(get, " ");
	//	cstring_write(get, to);	
	//	cstring_write(get, " ");
	//	cstring_write(get, cstring_FromInt(amount));
	// This is quite useless, we have to review if the map must know where they are.... I don't think so.
	return 0;
}






