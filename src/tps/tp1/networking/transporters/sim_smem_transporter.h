#include "../../sim_common_reqs.h"

#ifndef _SIM_SMEM_TRANSPORTER_H_
#define _SIM_SMEM_TRANSPORTER_H_

typedef struct sim_smem_transporter sim_smem_transporter;

sim_smem_transporter * sim_smem_transporter_init(int server_id, int client_id, int is_server);

void sim_smem_transporter_write(sim_smem_transporter * t, cstring data);

cstring sim_smem_transporter_listen(sim_smem_transporter * t, void_p extra_data);

void sim_smem_transporter_free(sim_smem_transporter * transp);

#endif