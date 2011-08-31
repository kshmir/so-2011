/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------------------------------------------------------------
 * sim_server acts as a one-to-many information distributer.
 * It binds string sequences to functions which process the data.
 * It can also send a query message to it's clients to make a specific petition.
 */

#include "../sim_common_reqs.h"

#ifndef _SIM_SERVER_H_
#define _SIM_SERVER_H_

typedef struct sim_server * sim_server;

typedef int connection_type;

typedef int process_type;

#include "sim_message.h"
#include "sim_client.h"
#include "sim_transporter.h"







/**
 Starts the server and it's local listener.
 */
sim_server sim_server_init(connection_type con, process_type p_type, int server_id);

/**
 Sends a broadcast query to it's clients.
 It's used for clients responding to actions like stop or tick.
 */
void sim_server_broadcast_query(sim_server s, cstring message);

/**
 Binds a char sequence to a receiver, it allows the server to responds to events.
 */
int sim_server_add_receiver(sim_server s, cstring sequence, sim_receiver f);

/**
 Creates a new client process and makes a transporter which can write to it.
 */
int sim_server_spawn_child(sim_server s);

/**
 Free's the server and it's thread.
 */
int sim_server_free(sim_server s);

#endif
