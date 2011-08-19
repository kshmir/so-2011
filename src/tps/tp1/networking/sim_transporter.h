#include "../sim_common_reqs.h"

#ifndef _SIM_TRANSPORTER_H_
#define _SIM_TRANSPORTER_H_

typedef int (*function)();

typedef struct sim_transporter * sim_transporter;

#include "sim_message.h"
#include "sim_server.h"

typedef int transporter_mode;

#define MODE_READWRITE			0
#define MODE_READ				1
#define MODE_WRITE				2

// Type of processes to open
#define P_AIRLINE				0
#define P_LEVEL					1
#define P_TESTER				2
#define P_TESTER_SERVER			3

// Type of connections
#define C_PIPE					0
#define C_SHARED_MEMORY			1
#define C_SOCKETS				2
#define C_M_QUEUES				3

sim_transporter sim_transporter_init(connection_type type,	// Tells the kind of IPC to use
									 process_type proc,		// Tells the kind of Process to open (filename)
									 int from_id,			// ID which starts the connection
									 int to_id,				// Endpoint
									 transporter_mode m,	// Mode: ReadWrite / Read / Write
									 int forks_child,		// Forks a process_type or not.
									 int is_server);		// If it's the server then it can open a process

void sim_transporter_free(sim_transporter transp);

#endif