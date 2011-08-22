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

#include "../sim_common_reqs.h"

#ifndef _SIM_MESSAGE_H_
#define _SIM_MESSAGE_H_

typedef struct sim_message * sim_message;

/**
	Typedef used in the callbacks of both server and client.
 	@example
		void example_server_receiver(sim_message mes) {
			cstring message = sim_message_read(mes);
			
			// process message.
 
			sim_message_set_header(mes, cstring_write(cstring_copy("RES ", sim_message_header(mes))));
			sim_message_write(mes, response);
			sim_message_respond(mes);
			pthread_cond_signal(&conds);
		}
	@param data Message data.
*/
typedef void (*sim_receiver)(sim_message data);

#ifndef _SIM_TRANSPORTER_H_
#include "sim_server.h"
#include "sim_client.h"
#endif

/**
 Starts the message, from a transporter, header and message.
 @param t Start transporter.
 @param header The message header, it can be null.
 @param string The message, it can also be null.
 */
sim_message sim_message_init(sim_transporter t, cstring header, cstring string);


/**
 * Accesor for the message.
 */
cstring sim_message_read(sim_message r);

/**
 * Frees up the message header and sets it with a new one.
 */
int sim_message_set_header(sim_message r, cstring header);

/**
 * Sends the message in a quirky way... it opens a thread and waits for it to receive the response
 * of the sent message.
 */
sim_message sim_message_send(sim_message r);

/**
 *	Just sends a message
 */
void sim_message_respond(sim_message r);

/**
 * Sets the message (does not append anything, beware)
 */
int sim_message_write(sim_message r, cstring data);

int sim_message_free(sim_message r);

#endif