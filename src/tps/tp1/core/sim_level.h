#include "../sim_common_reqs.h"

#ifndef _SIM_LEVEL_H_
#define _SIM_LEVEL_H_

#include "../networking/sim_client.h"
#include "../networking/sim_server.h"

typedef struct sim_level * sim_level;

cstring sim_level_serialize(sim_level lev);

sim_level sim_level_deserialize(cstring s);


int sim_level_has_city(sim_level lev, cstring city);


void sim_level_free(sim_level lev);

void sim_level_main(sim_level lev);

#endif