#include "../sim_common_reqs.h"

#ifndef _SIM_TRANSPORTER_H_
#define _SIM_TRANSPORTER_H_

typedef int (*function)(void_p);

typedef struct sim_transporter * sim_transporter;

// Se conecta al servidor.
// Usado por sim_client
sim_transporter sim_transporter_init(connection_type type, int connection_id);

// Forkea el tipo de proceso dado y le pasa la información para arrancar.
// Usado por sim_server
sim_transporter sim_transporter_fork(connection_type type, process_type proc, int connection_id);

void sim_transporter_write(sim_transporter sim);

cstring sim_transporter_listen(connection_type type, void_p extra_data);


void sim_transporter_free(sim_transporter transp);

#endif