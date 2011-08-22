#include "../sim_common_reqs.h"

#ifndef _SIM_AIRLINE_H_
#define _SIM_AIRLINE_H_

#include "sim_plane.h"
#include "../networking/sim_client.h"

typedef struct sim_airline * sim_airline;


list sim_airline_planes(sim_airline air);

sim_airline sim_airline_deserialize(cstring s, int airline_id);

cstring sim_airline_serialize(sim_airline s, int hasId);

void sim_airline_main(sim_client c, int airline_id);

#endif
