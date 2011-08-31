#include "../sim_common_reqs.h"

#ifndef _SIM_PLANE_H_
#define _SIM_PLANE_H_

#include "sim_extra.h"
#include "sim_airline.h"
#include "sim_level.h"

typedef struct sim_plane * sim_plane;

struct sim_plane_data {
	void_p		plane;
	void_p		airline;
	void_p		level;
};

int sim_plane_comparer(void_p a1, void_p a2);

int sim_plane_set_id(sim_plane p, int i);

cstring sim_plane_start_city(sim_plane p);

void sim_plane_main(struct sim_plane_data * d);


#endif