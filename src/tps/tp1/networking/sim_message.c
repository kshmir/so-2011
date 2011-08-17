#include "sim_message.h"
#include "sim_transporter.h"

#include <time.h>
#include <pthread.h>

struct sim_message {
	cstring			message;
	sim_transporter t;
	pthread_t		thread;
	pthread_attr_t	attr;
};

sim_message sim_message_init(sim_transporter t, cstring string) {
	sim_message m = (sim_message) malloc(sizeof(struct sim_message));
	m->t = t;
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
	free(r->message);
	free(r);
}


static void_p sim_message_listen(sim_message r) {
	int found = 0;
	cstring old_msg = r->message;
	while (!found) {
		cstring msg = sim_transporter_listen(r->t);
	}

}

sim_message sim_message_send(sim_message r) {
	pthread_create(&r->thread, &r->attr, (void_p) sim_message_listen, (void_p) r);
	sim_message_respond(r);
	pthread_join(r->thread, NULL);
}


void sim_message_respond(sim_message r) {
	sim_transporter_write(r->t, r->message);
}