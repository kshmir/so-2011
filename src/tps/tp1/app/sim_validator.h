/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 *
 * @file sim_validator.h
 *
 * @brief Validates all data and sets it up to be transferred to the level.
 *
 * @author Agustin Marseillan
 * @author Maximo Videla
 * @author Cristian Pereyra
 */

#include "../sim_common_reqs.h"
#include "../networking/sim_transporter.h"

#ifndef _SIM_FRONTEND_H_
#define _SIM_FRONTEND_H_


/**
 * Validates the level.
 *
 * @param path Path to the file to validate.
 * @param line_error Line in which the error might appear.
 *
 * @return Valid level, or NULL if not valid.
 */
int sim_validator_params(list args, cstring * return_level, list return_airline, cstring * error, connection_type * c_type);

/**
 * Validates the level.
 *
 * @param path Path to the file to validate.
 * @param line_error Line in which the error might appear.
 *
 * @return Valid level, or NULL if not valid.
 */
sim_level sim_validator_level(cstring path,int * line_error);

/**
 * Validates an airline.
 *
 * @param path	Path to the file to validate.
 * @param lev	Owner level.
 * @param line_error Line in which the error might appear.
 *
 * @return Valid airline, or NULL if not valid.
 */
sim_airline sim_validator_airline(cstring path, sim_level level, int* line_error);

#endif
