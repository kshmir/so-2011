#ifndef _SIM_SMEM_TRANSPORTER_H_
#define _SIM_SMEM_TRANSPORTER_H_

typedef struct sim_smem_transporter * sim_smem_transporter;

// Se conecta al servidor.
// Usado por sim_client
sim_smem_transporter sim_smem_transporter_init();

sim_smem_transporter sim_smem_transporter_open(int connection_id);

void sim_transporter_write(sim_transporter sim);

cstring sim_transporter_listen(connection_type type, void_p extra_data);


void sim_transporter_free(sim_transporter transp);

#endif