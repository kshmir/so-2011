/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 *
 * @file sim_airline.h
 *
 * @brief Handles all the logic of all the airlines.
 *
 * @author Agustin Marseillan
 * @author Maximo Videla
 * @author Cristian Pereyra
 */


#include "../sim_common_reqs.h"

#ifndef _SIM_AIRLINE_H_
#define _SIM_AIRLINE_H_

#include "sim_plane.h"
#include "../networking/sim_client.h"

typedef struct sim_airline * sim_airline;

/**
 * Getter for the airline client.
 */
sim_client sim_airline_client(sim_airline air);

/**
 * Airline planes.
 */
list sim_airline_planes(sim_airline air);

/**
 * Deserializes the airline.
 *
 * @param s Serialized string
 * @param airline_id Airline to serialize
 *
 * @return Fully built airline.
 */
sim_airline sim_airline_deserialize(cstring s, int airline_id);

/**
 * Deserializes the airline and tells the line it fails on.
 *
 * @param s Serialized string
 * @param airline_id Airline to serialize
 * @param err_line Line of the error.
 *
 * @return Fully built airline.
 */
sim_airline sim_airline_deserialize_line_error(cstring s, int airline_id, int *err_line);

/**
 * Serializes the airline
 *
 * @param air Airline to serialize
 * @param hasId Serialize with it's id or not.
 *
 * @return Fully built airline.
 */
cstring sim_airline_serialize(sim_airline s, int hasId);

/**
 * Airline main.
 *
 * @param connection_t	Connection type
 * @param from_id		Id of it's owner.
 * @param to_id			Id of the airline owner.
 */
void sim_airline_main(int connection_t, int from_id, int to_id);

/**
 * Game iteration.
 */
void sim_airline_free(sim_airline s);

/**
 * Getter for the airline mutex.
 */
pthread_mutex_t * sim_airline_mutex(sim_airline air);

/**
 * Getter for the airline waiting cond.
 */
pthread_cond_t * sim_airline_waiting_cond(sim_airline air);

/**
 * Getter for the planes moving.
 */
pthread_cond_t * sim_airline_planes_cond(sim_airline air);		
#endif
