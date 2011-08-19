#include "../../sim_common_reqs.h"

#ifndef _SIM_pipe_TRANSPORTER_H_
#define _SIM_pipe_TRANSPORTER_H_

typedef struct sim_pipe_transporter * sim_pipe_transporter;


sim_pipe_transporter sim_pipe_transporter_init_client(int server_id, int client_id);

sim_pipe_transporter sim_pipe_transporter_init_server(int client_id, int server_id, transporter_mode mode);

void sim_pipe_transporter_write(sim_pipe_transporter t, cstring data);

cstring sim_pipe_transporter_listen(sim_pipe_transporter t, int * extra_data);

void sim_pipe_transporter_free(sim_pipe_transporter transp);

#endif