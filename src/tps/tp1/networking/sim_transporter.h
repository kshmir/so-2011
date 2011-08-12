#include "../sim_common_reqs.h"

#ifndef _SIM_TRANSPORTER_H_
#define _SIM_TRANSPORTER_H_

typedef int (*function)();

typedef struct sim_transporter * sim_transporter;

#include "sim_message.h"
#include "sim_server.h"


// Type of processes to open
#define P_AIRLINE				0
#define P_LEVEL					1
#define P_TESTER				2

// Type of connections
#define C_PIPE					0
#define C_SHARED_MEMORY			1
#define C_SOCKETS				2
#define C_M_QUEUES				3
#define C_LOCAL_MEMORY			4

// Se conecta al servidor.
// Usado por sim_client
sim_transporter sim_transporter_init(connection_type type, int from_id, int to_id);

// Forkea el tipo de proceso dado y le pasa la información para arrancar.
// Usado por sim_server
sim_transporter sim_transporter_fork(connection_type type, process_type proc, int from_id, int to_id);



void sim_transporter_free(sim_transporter transp);

#endif