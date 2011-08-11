#include "../sim_common_reqs.h"

#ifndef _SIM_AIRLINE_H_
#define _SIM_AIRLINE_H_

typedef struct sim_airline * sim_airline;

int sim_airline_add_plane(sim_airline air, cstring name, sim_capacity c);

list sim_airline_planes(sim_airline air);

sim_airline sim_airline_deserialize(cstring s, int airline_id);

cstring sim_airline_serialize(sim_airline s);

void sim_airline_main(sim_airline air);

#endif
