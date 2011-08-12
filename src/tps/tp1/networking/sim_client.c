#include "sim_client.h"

struct sim_client {
	sim_transporter t;
	sim_receiver	r;
};

sim_client sim_client_init(sim_transporter t, sim_receiver r) {
	sim_client c = (sim_client) malloc(sizeof(sim_client));
	c->t = t;
	c->r = r;
	return c;
}

int sim_client_get_distance(sim_client c, int object_id, cstring from, cstring to) {
	cstring get = cstring_fromInt(object_id); 
	cstring_copy(" GET DIST ");
	cstring_write(get, from);
	cstring_write(get, " ");
	cstring_write(get, to);
	sim_message request = sim_message_init(c->t, get);
	sim_message response = sim_message_send(request);
	free(request);
	// Rebuild response
	// Response should be... {object_id} RES DIST {from} {to} INT_VALUE
	// We should send INT_VALUE.
	return 0;
}

map sim_client_get_medicines(sim_client c, int object_id, cstring from) {
	cstring get = cstring_fromInt(object_id); 
	cstring_copy(" GET MEDS ");
	cstring_write(get, " ");
	cstring_write(get, from);
	sim_message request = sim_message_init(c->t, get);
	sim_message response = sim_message_send(request);
	free(request);
	// Rebuild map from response
	// Response should be... {object_id} RES MEDS \n\n(list of medicines as saved in files)
	
	return NULL;
}


int sim_client_post_medicine_fill(sim_client c, int object_id, cstring city, cstring medicine, int amount) {
	cstring get = cstring_fromInt(object_id); 
	cstring_copy(" POST MEDF ");
	cstring_write(get, " ");
	cstring_write(get, city);
	cstring_write(get, " ");
	cstring_write(get, medicine);	
	cstring_write(get, " ");
	cstring_write(get, cstring_fromInt(amount));
	sim_message request = sim_message_init(c->t, get);
	sim_message response = sim_message_send(request);
	free(request);
	// Rebuild response
	// Response should be... {object_id} RES MEDF {city} {medicine} {value}
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






