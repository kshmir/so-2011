#include "../sim_common_reqs.h"

#ifndef _SIM_CLIENT_H_
#define _SIM_CLIENT_H_

#include "sim_message.h"
#include "sim_server.h"
#include "sim_transporter.h"

typedef struct sim_client * sim_client;

sim_client sim_client_init(sim_transporter t, sim_receiver query_receiver);

// GET requests of params.

int sim_client_get_distances(sim_client c, int object_id, cstring from, cstring to);

map sim_client_get_medicines(sim_client c, int object_id, cstring from);


// POST request of params.

int sim_client_post_medicine_fill(sim_client c, int object_id, cstring city, cstring medicine, int amount);

void sim_client_free(sim_client c);

#endif