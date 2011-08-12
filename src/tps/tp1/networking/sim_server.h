#include "../sim_common_reqs.h"

#ifndef _SIM_SERVER_H_
#define _SIM_SERVER_H_

typedef struct sim_server * sim_server;

typedef int connection_type;

typedef int process_type;

#include "sim_message.h"
#include "sim_client.h"
#include "sim_transporter.h"

sim_server sim_server_init(connection_type con);

void sim_server_listen();

int sim_server_add_receiver(sim_server s, cstring sequence, sim_receiver f);

#endif