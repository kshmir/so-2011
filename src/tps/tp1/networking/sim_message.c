#include "sim_message.h"
#include "sim_transporter.h"

#include <time.h>
#include <pthread.h>

struct sim_message {
	cstring			header;
	cstring			message;
	sim_transporter t;
	pthread_t		thread;
	pthread_attr_t	attr;
};

sim_message sim_message_init(sim_transporter t, cstring header, cstring string) {
	sim_message m = (sim_message) malloc(sizeof(struct sim_message));
	m->t = t;
	if (header == NULL) {
		m->header = cstring_init(0);		
	}
	else {
		m->header = header;
	}

	if (string == NULL)
		m->message = cstring_init(0);
	else {
		m->message = string;
	}
	
	pthread_attr_init(&m->attr);
	pthread_attr_setdetachstate(&m->attr, PTHREAD_CREATE_JOINABLE);
	return m;
}

cstring sim_message_read(sim_message r) {
	cstring resp = r->message;
	r->message = NULL;
	return resp;
}

int sim_message_write(sim_message r, cstring data) {
	if (r->message) {
		r->message = cstring_init(0);
	}
	
	r->message = cstring_write(r->message, data);
	return 1;
}

int sim_message_free(sim_message r) {
	free(r);
}


struct listener_data {
	sim_message 	r;
	sim_message   	m;
};

static void_p sim_message_listen(struct listener_data * data) {
	sim_message r			= data->r;
	int found = 0;
	cstring old_msg = r->message;
	while (!found) {
		cstring msg = sim_transporter_listen(r->t);
		cstring header = cstring_copy_until_char(msg, ';');
		
		if (msg[0] == 0) {
			return NULL; 
		}
		
		if (cstring_matches(header,"RES ")) {
			cstring no_resp = cstring_replace(msg, "RES ", "");
			list splitted = cstring_split_list(no_resp, ";");
			
			if (list_size(splitted) == 2) {
				if (cstring_compare(list_get(splitted, 0), r->header) == 0) {
					found = 1;
					data->m = sim_message_init(r->t,list_get(splitted, 0), list_get(splitted, 1));
					sim_transporter_dequeue(r->t);
				}
			}
			list_free(splitted);
			free(no_resp);
		} 
		free(header);
		if (msg != NULL) {
			free(msg);
		}
	}
	return 0;
}

sim_message sim_message_send(sim_message r) {
	struct listener_data data;
	data.r = r;
	pthread_create(&r->thread, &r->attr, (void_p) sim_message_listen, (void_p) &data);
	sim_message_respond(r);
	pthread_join(r->thread, NULL);
	sim_message_free(r);
	sim_message response = data.m;
	return response;
}


void sim_message_respond(sim_message r) {	
	cstring array[3];
	array[0] = r->header;
	array[1] = r->message;
	array[2] = NULL;
	sim_transporter_write(r->t, cstring_join(array, ";"));
}