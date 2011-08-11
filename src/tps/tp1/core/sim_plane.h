#include "../sim_common_reqs.h"

#ifndef _SIM_PLANE_H_
#define _SIM_PLANE_H_

typedef struct sim_plane * sim_plane;

int sim_plane_comparer(void_p a1, void_p a2);

sim_plane sim_plane_deserialize(cstring s);

cstring sim_plane_serialize(sim_plane s);

void sim_plane_main(sim_plane air);

#endif