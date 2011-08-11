#include "../sim_common_reqs.h"

#include <time.h>

struct sim_message {
	cstring message;
	time_t timestamp;
	sim_transporter t;
}

sim_message sim_message_init(sim_transporter t, cstring string) {
	sim_message m = (sim_message) malloc(sizeof(struct sim_message));
	m->timestamp = time(0);
	m->t = t;
	if (string == NULL)
		m->message = cstring_init(0);
	else {
		m->message = string;
	}
	return m;
}

sim_message sim_message_request_and_response(sim_transporter t, cstring request) {
	sim_message _request = sim_message_init(t, request);
	sim_message response = sim_message_send(_request);
	sim_message_free(_request);
	return response;
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

int sim_message_free(sim_response r) {
	free(r->message);
	free(r);
}


sim_message sim_message_send(sim_message r) {
	// Send message to transporter and receive data.
	// Must use mutex.
}


void sim_message_respond(sim_message r) {
	// Send message to transporter, but don't wait to receive data.
	// This is used by sim_server on all responses
	// And by sim_client when receiving QUERY.
}