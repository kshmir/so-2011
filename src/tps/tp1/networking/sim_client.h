#include "../sim_common_reqs.h"

#ifndef _SIM_CLIENT_H_
#define _SIM_CLIENT_H_

typedef struct sim_client * sim_client;

sim_client sim_client_init(receiver query_receiver);


// GET requests of params.

int sim_client_get_distances(sim_client c, cstring from, cstring to);

map sim_client_get_medicines(sim_client c, cstring from);


// POST request of params.

int sim_client_post_medicine_fill(sim_client c, int airline_id, int amount);

void sim_client_free(sim_client c);

#endif