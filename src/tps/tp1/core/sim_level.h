#include "../sim_common_reqs.h"

#ifndef _SIM_LEVEL_H_
#define _SIM_LEVEL_H_

typedef struct sim_level * sim_level;

cstring sim_level_serialize(sim_level lev);

sim_level sim_level_deserialize(cstring s);

void sim_level_main(sim_level lev);

#endif