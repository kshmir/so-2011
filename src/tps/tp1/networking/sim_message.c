/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------------------------------------------------------------
 * sim_message is used to make requests from the client to the server.
 * It allows both server and client to communicate transparently via callbacks.
 * It also allows an abstraction layer from the transporter.
 * It makes easier the implementation of our protocol, in which messages are sent in the format:
 *		MESSAGE_HEADER;MESAGE_BODY
 */


#include "sim_message.h"
#include "sim_transporter.h"

#include <time.h>
#include <pthread.h>

/**
	Data structure used to send messages through client/server.
 */
struct sim_message {
	cstring			header;			// Message header. Used for recognizing the message.
	cstring			message;		// Actual message. It's splitted with the header vía a ';'
	sim_transporter t;				// Transporter.
	pthread_t		thread;			// Thread used for listening the message and sending it at the sa same time.
	pthread_attr_t	attr;			// Makes the thread joinable.
};

/**
	Starts the message, from a transporter, header and message.
	@param t Start transporter.
	@param header The message header, it can be null.
	@param string The message, it can also be null.
 */
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

cstring sim_message_header(sim_message r) {
	return r->header;
}

/**
 * Accesor for the message.
 */
cstring sim_message_read(sim_message r) {
	return r->message;
}

/**
 * Frees up the message header and sets it with a new one.
 */
int sim_message_set_header(sim_message r, cstring header) {
	if (r->header != NULL) {
		free(r->header);
	}
	r->header = cstring_copy(header);	
	return 1;
}

/**
 * Sets the message (does not append anything, beware)
 */
int sim_message_write(sim_message r, cstring data) {
	if (r->message != NULL) {
		free(r->message);
	}
	r->message = cstring_copy(data);
	return 1;
}

int sim_message_free(sim_message r) {
//	free(r->message);
//	free(r->header);
//	free(r);
}

/**
 * Used for the message listener.
 */
struct listener_data {
	sim_message 	r;		// Request message.
	sim_message   	m;		// Response message.
};

/**
 * Listens until it gets the desired message.
 */
static void_p sim_message_listen(struct listener_data * data) {
	sim_message r			= data->r;
	int found = 0;
	cstring old_msg = r->message;
	cstring last_msg = NULL;
	while (!found) {
		cstring msg = sim_transporter_listen(r->t, NULL);
		cstring header = cstring_copy_until_char(msg, ';');
		
//		if (msg[0] == 0) {
//			return NULL; 
//		}
		
//		cprintf("MSG GETS: %s expects %s\n", AMARILLO, msg, r->header);
		if (cstring_matches(header,"RES ")) {
			cstring no_resp = cstring_replace(msg, "RES ", "");
			list splitted = cstring_split_list(no_resp, ";");
			
			
			
			if (list_size(splitted) == 2)
			{
				
				if (cstring_compare(list_get(splitted, 0), r->header) == 0) {
					found = 1;
					cstring _header = cstring_copy(list_get(splitted, 0));
					cstring	_message = cstring_copy(list_get(splitted, 1));
					data->m = sim_message_init(r->t, _header, _message);
					sim_transporter_dequeue(r->t);
					list_free_with_data(splitted);
					free(no_resp);					
					free(header);
					free(msg);
					return 0;
				}
			}
			if (last_msg != NULL) {
				free(last_msg);
			}
			last_msg = NULL;
			list_free_with_data(splitted);
			free(no_resp);
		}  else {
			if (last_msg != NULL) {
				free(last_msg);
			}
			last_msg = msg;
		}
		free(header);
	}
	return 0;
}

/**
 * Sends the message in a quirky way... it opens a thread and waits for it to receive the response
 * of the sent message.
 */
sim_message sim_message_send(sim_message r) {
	
	// Builds up the structure.
	struct listener_data data;
	data.r = r;
	
	
	// Starts the listener of the message.
	pthread_create(&r->thread, &r->attr, (void_p) sim_message_listen, (void_p) &data);

	cstring array[3];
	array[0] = r->header;
	array[1] = r->message;
	array[2] = NULL;
//	cprintf("MSG SENDS: %s\n", VIOLETA, r->header);
	cstring joined = cstring_join(array, ";");

	sim_transporter_write(r->t, joined);
	free(joined);

	// Waits for the data.
	pthread_join(r->thread, NULL);

	
	// Clears the message and returns the response.
	sim_message_free(r);
	sim_message response = data.m;

	return response;
}

/**
 *	Just sends a message
 */
void sim_message_respond(sim_message r) {	
	cstring array[3];
	array[0] = cstring_copy(r->header);
	array[1] = cstring_copy(r->message);
	array[2] = NULL;

	cstring joined = cstring_join(array, ";");
//	cprintf("SERVER: SEND %s\n", ROJO, joined);
	sim_transporter_write(r->t, joined);
	free(array[0]);
	free(array[1]);
	free(joined);
}