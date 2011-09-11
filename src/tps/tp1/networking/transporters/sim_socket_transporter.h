/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------------------------------------------------------------
 * FIFO implementation of transporter.
 */

#include "../../sim_common_reqs.h"

#ifndef _SIM_socket_TRANSPORTER_H_
#define _SIM_socket_TRANSPORTER_H_

typedef struct sim_socket_transporter * sim_socket_transporter;

/*
 * Starts a client and waits until it is connected to the server. Returns the client information
 * in the socket transporter.
 * */
sim_socket_transporter sim_socket_transporter_init_client(int server_id, int client_id);

/*
 * Starts a server and waits until a client requests a connection. Returns the client information
 * in the socket transporter.
 * */
sim_socket_transporter sim_socket_transporter_init_server(int client_id, int server_id);

/*
 * Writes the data threw the transporter
 * */
void sim_socket_transporter_write(sim_socket_transporter t, cstring data);

/*
 * Reads the data received by transporter
 * */
cstring sim_socket_transporter_listen(sim_socket_transporter t, int * extra_data);

/*
 * Frees all the resources.
 * */
void sim_socket_transporter_free(sim_socket_transporter transp);

#endif
