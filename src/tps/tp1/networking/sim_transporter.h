#include "../sim_common_reqs.h"

#ifndef _SIM_TRANSPORTER_H_
#define _SIM_TRANSPORTER_H_

typedef int (*function)();

typedef struct sim_transporter * sim_transporter;

#include "sim_message.h"
#include "sim_server.h"

// Se conecta al servidor.
// Usado por sim_client
sim_transporter sim_transporter_init(connection_type type, int from_id, int to_id);

// Forkea el tipo de proceso dado y le pasa la información para arrancar.
// Usado por sim_server
sim_transporter sim_transporter_fork(connection_type type, process_type proc, int from_id, int to_id);



void sim_transporter_free(sim_transporter transp);

#endif