#include "../sim_common_reqs.h"

#ifndef _SIM_EXTRA_H_
#define _SIM_EXTRA_H_

/**
	keypair 
 **/

struct sim_keypair {
	cstring	name;
	int		amount;
};

typedef struct sim_keypair * sim_keypair;

cstring sim_keypair_serialize(sim_keypair c);

sim_keypair sim_keypair_deserialize(cstring line);

void sim_keypair_free(sim_keypair c);


/**
	map relation
 **/

struct sim_map_relation {
	cstring	from;
	cstring	to;
	int		weight;
};

typedef struct sim_map_relation * sim_map_relation;

cstring sim_map_relation_serialize(sim_map_relation c);

sim_map_relation sim_map_relation_deserialize(cstring line);

void sim_keypair_free(sim_keypair c);

#endif
