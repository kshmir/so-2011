#include "../sim_common_reqs.h"

#ifndef _SIM_PLANE_H_
#define _SIM_PLANE_H_

#include "sim_extra.h"

typedef struct sim_plane * sim_plane;

int sim_plane_comparer(void_p a1, void_p a2);

cstring sim_plane_start_city(sim_plane p);

void sim_plane_main(sim_plane air);


#endif