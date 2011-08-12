#include "../sim_common_reqs.h"

#ifndef _SIM_SERVER_H_
#define _SIM_SERVER_H_

typedef struct sim_server * sim_server;

typedef int connection_type;

typedef int process_type;

#include "sim_message.h"
#include "sim_client.h"
#include "sim_transporter.h"

// Type of processes to open
#define P_AIRLINE				0
#define P_LEVEL					1
#define P_TESTER				2

// Type of connections

#define C_PIPE					0
#define C_SHARED_MEMORY			1
#define C_SOCKETS				2
#define C_M_QUEUES				3



sim_server sim_server_init(connection_type con);

// Starts a new client, sending the process type as an arg and the connection type.
int	sim_server_fork(process_type proc);

int sim_server_add_receiver(sim_server s, cstring sequence, sim_receiver f);

#endif