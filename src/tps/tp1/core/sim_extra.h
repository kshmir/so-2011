/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 *
 * @file sim_extra.h
 *
 * @brief Structures used to help the serializing of the structures.
 *
 * @author Agustin Marseillan
 * @author Maximo Videla
 * @author Cristian Pereyra
 */
#include "../sim_common_reqs.h"

#ifndef _SIM_EXTRA_H_
#define _SIM_EXTRA_H_

/**
	Keypair represents the data stored as: "name amount"
 */

struct sim_keypair {
	cstring	name;
	int		amount;
};

typedef struct sim_keypair * sim_keypair;

/**
 * Serializes the keypair
 */
cstring sim_keypair_serialize(sim_keypair c);

/**
 * Deserializes the keypair, returns NULL if invalid.
 */
sim_keypair sim_keypair_deserialize(cstring line);

/**
 * Frees the keypair.
 */
void sim_keypair_free(sim_keypair c);


/**
	Map relation represents the data stored as: "from to weight"
 **/

struct sim_map_relation {
	cstring	from;
	cstring	to;
	int		weight;
};

typedef struct sim_map_relation * sim_map_relation;

/**
 * Serializes a relation.
 */
cstring sim_map_relation_serialize(sim_map_relation c);

/**
 * Deserializes a relation.
 */
sim_map_relation sim_map_relation_deserialize(cstring line);

/**
 * Free's up a relation.
 */
void sim_keypair_free(sim_keypair c);

#endif
