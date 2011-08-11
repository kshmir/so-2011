#include "../sim_common_reqs.h"

#ifndef _SIM_SERVER_H_
#define _SIM_SERVER_H_

// Type of processes to open
#define P_AIRLINE				0
#define P_LEVEL					1
#define P_TESTER				9

// Type of connections

#define C_PIPE					0
#define C_SHARED_MEMORY			1
#define C_SOCKETS				2
#define C_M_QUEUES				3

typedef struct sim_server * sim_server;

typedef struct int connection_type;

typedef struct int process_type;

sim_server sim_server_init(connection_type con);

// Starts a new client, sending the process type as an arg and the connection type.
int	sim_server_fork(process_type proc);

int sim_server_add_receiver(sim_server s, cstring sequence, sim_receiver function);

#endif