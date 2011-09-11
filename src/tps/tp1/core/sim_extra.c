/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 *
 * @file sim_extra.c
 *
 * @brief Structures used to help the serializing of the structures.
 *
 * @author Agustin Marseillan
 * @author Maximo Videla
 * @author Cristian Pereyra
 */

#include "sim_extra.h"

/**
 * Starts a keypair of data.
 */
sim_keypair sim_keypair_init(cstring name, int weight) {
	sim_keypair c = (sim_keypair) malloc(sizeof(struct sim_keypair));
	c->name = cstring_copy(name);
	c->amount = weight;
	return c;
}

/**
 * Serializes the keypair
 */
cstring sim_keypair_serialize(sim_keypair c) {		
	cstring params[3];
	cstring aux;
	params[0] = c->name;
	params[1] = cstring_fromInt(c->amount);
	params[2] = NULL; // BEWARE! ;)
	aux=cstring_join(params, " ");
	return aux;
}

/**
 * Deserializes the keypair, returns NULL if invalid.
 */
sim_keypair sim_keypair_deserialize(cstring line) {
	cstring * array = cstring_split(line," ");
	int i = 0;
	while(array[i] != NULL) {
		i++;
	}
	if (i == 2) {
		int _ret = 0;
		int weight = cstring_parseInt(array[1],&_ret);
		if (_ret == 1) {
			sim_keypair kp = sim_keypair_init(array[0],weight);
			for(; i >= 0; i--) {
				free(array[i]);
			}
			free(array);
			return kp;
		} else {
			for(; i >= 0; i--) {
				free(array[i]);
			}
			free(array);
			return NULL;
		}
	}
	else {
		for(; i >= 0; i--) {
			free(array[i]);
		}
		free(array);
		return NULL;
	}
}

/**
 * Frees the keypair.
 */
void sim_keypair_free(sim_keypair c) {
	free(c->name);
	free(c);
}


/**
 * Starts a relation 
 */
static sim_map_relation sim_relation_init(cstring from, cstring to, int weight) {
	sim_map_relation c = (sim_map_relation) malloc(sizeof(struct sim_map_relation));
	c->from = cstring_copy(from);
	c->to   = cstring_copy(to);
	c->weight = weight;
	return c;
}

/**
 * Serializes a relation.
 */
cstring sim_map_relation_serialize(sim_map_relation c) {
	cstring params[4];
	
	params[0] = c->from;
	params[1] = c->to;
	params[2] = cstring_fromInt(c->weight);
	params[3] = NULL; // BEWARE! ;)
	
	return cstring_join(params, " ");
}

/**
 * Deserializes a relation.
 */
sim_map_relation sim_map_relation_deserialize(cstring line) {
	cstring * array = cstring_split(line," ");
	int i = 0;
	while(array[i] != NULL) {
		i++;
	}
	if (i == 3) {
		int _ret = 0;
		int weight = cstring_parseInt(array[2],&_ret);
		if (_ret == 1) {
			sim_map_relation rel = sim_relation_init(array[0], array[1], weight);
			for(; i >= 0; i--) {
				free(array[i]);
			}
			free(array);
			return rel;
		} else {
			for(; i >= 0; i--) {
				free(array[i]);
			}
			free(array);
			return NULL;
		}
	}
	else {
		for(; i >= 0; i--) {
			free(array[i]);
		}
		free(array);
		return NULL;
	}	
}

/**
 * Free's up a relation.
 */
void sim_relation_free(sim_map_relation c) { 
	free(c->from);
	free(c->to);
	free(c);
}
