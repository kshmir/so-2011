#include "../sim_common_reqs.h"

#ifndef _SIM_LEVEL_H_
#define _SIM_LEVEL_H_

#include "../networking/sim_client.h"
#include "../networking/sim_server.h"

typedef struct sim_level * sim_level;

/**
 *  Getter for the level.
 */
graph sim_level_graph(sim_level lev);

/**
 *  Serializes a level.
 */
cstring sim_level_serialize(sim_level lev);

/**
 *  Deserializes a level.
 */
sim_level sim_level_deserialize(cstring s);

/**
 *  Deserializes a level and returns the line it fails on
 */
sim_level sim_level_deserialize_line_error(cstring s,int* err_line);

/**
 *  Checks for the existance of a city.
 */
int sim_level_has_city(sim_level lev, cstring city);

/**
 *  Free's a level
 */
void sim_level_free(sim_level lev);

/**
 * Starts the level, asks for the data and locks on the logic, while serving for events
 */
void sim_level_main(int connection_t, int from_id, int to_id);

#endif
