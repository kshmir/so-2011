#include "../../sim_common_reqs.h"

#ifndef _SIM_socket_TRANSPORTER_H_
#define _SIM_socket_TRANSPORTER_H_

typedef struct sim_socket_transporter * sim_socket_transporter;

sim_socket_transporter sim_socket_transporter_init_client(int server_id, int client_id);

sim_socket_transporter sim_socket_transporter_init_server(int client_id, int server_id);

void sim_socket_transporter_write(sim_socket_transporter t, cstring data);

cstring sim_socket_transporter_listen(sim_socket_transporter t, int * extra_data);

void sim_socket_transporter_free(sim_socket_transporter transp);

#endif
