#include "../sim_common_reqs.h"

#ifndef _SIM_AIRLINE_H_
#define _SIM_AIRLINE_H_

#include "sim_plane.h"
#include "../networking/sim_client.h"

typedef struct sim_airline * sim_airline;

sim_client sim_airline_client(sim_airline air);

list sim_airline_planes(sim_airline air);

sim_airline sim_airline_deserialize(cstring s, int airline_id);

sim_airline sim_airline_deserialize_line_error(cstring s, int airline_id, int *err_line);

cstring sim_airline_serialize(sim_airline s, int hasId);

void sim_airline_main(int connection_t, int from_id, int to_id);













pthread_mutex_t * sim_airline_mutex(sim_airline air);

pthread_cond_t * sim_airline_waiting_cond(sim_airline air);

pthread_cond_t * sim_airline_planes_cond(sim_airline air);		


#endif
