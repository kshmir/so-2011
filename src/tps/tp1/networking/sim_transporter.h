/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------------------------------------------------------------
 * sim_transporter adt, read the .c file to see what it does deeply.
 */


#include "../sim_common_reqs.h"


#ifndef _SIM_TRANSPORTER_H_
#define _SIM_TRANSPORTER_H_

typedef int (*function)();

typedef struct sim_transporter * sim_transporter;

#include "sim_message.h"
#include "sim_server.h"

typedef int transporter_mode;

// Types of modes to operate
#define MODE_READWRITE			0
#define MODE_READ				1
#define MODE_WRITE				2

// Types of processes to open
#define P_AIRLINE				0
#define P_LEVEL					1
#define P_TESTER				2
#define P_TESTER_SERVER			3

// Types of connections
#define C_PIPE					0
#define C_M_QUEUES				1
#define C_SOCKETS				2
#define C_SHARED_MEMORY			3



/**
 Starts a new transporter and opens the connections.
 It can fork a process or start as a client. 
 @param type The type of IPC to use.
 @param proc The type of process to open.
 @param from_id The id from which the transporter is started
 @param to_id   The id to which the transporter is connected, it might create it if it forks_child
 @param mode    The mode with with the transporter operates, READ, READWRITE or WRITE.
 @param forks_child Tells wether the transporter forks a new child. It must be true and must have is_server as true.
 @param is_server It tells if it forks a server.
 @return The local endpoint of the transporter built, with the listener on if it's set to read.
 */

sim_transporter sim_transporter_init(connection_type type,	// Tells the kind of IPC to use
									 process_type proc,		// Tells the kind of Process to open (filename)
									 int from_id,			// ID which starts the connection
									 int to_id,				// Endpoint
									 transporter_mode m,	// Mode: ReadWrite / Read / Write
									 int forks_child,		// Forks a process_type or not.
									 int is_server);		// If it's the server then it can open a process

/**
 Accesor for client_id, it is used in upper levels of the IPC implementation, like server or client.
 @param t The transporter
 @return The client_id of the transporter.
 */
int sim_transporter_client_id(sim_transporter t);

/**
 Accesor for server_id, it is used in upper levels of the IPC implementation, like server or client.
 @param t The transporter
 @return The server_id of the transporter.
 */
int sim_transporter_server_id(sim_transporter t);

/**
 Takes off the last message read from the queue. This means it's accepted by it's subscriber.
 TWO subscribers ** shouldn't ** accept the same message at the same time.
 */
void sim_transporter_dequeue(sim_transporter t);

/**
 Listens and locks until there is something to read.
 It *** might *** always return the same string until another process has read it successfully. 
 It would be great to improve this.
 */
cstring sim_transporter_listen(sim_transporter t, cstring avoid);

/**
 Writes a message to the current transporter.
 */
void sim_transporter_write(sim_transporter sim, cstring data);

/**
 Free's up a transporter
 */
void sim_transporter_free(sim_transporter transp);

#endif