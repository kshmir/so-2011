/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 *
 * @file sim_frontend.h
 *
 * @brief Shows the functions used by the frontend, just the main, all the rest are static.
 *
 * @author Agustin Marseillan
 * @author Maximo Videla
 * @author Cristian Pereyra
 *
 */

#include "../sim_common_reqs.h"
#include "../networking/sim_transporter.h"

#ifndef _SIM_FRONTEND_H_
#define _SIM_FRONTEND_H_

/**
 * Starts the frontend logic, which validates the data and starts the level.
 *
 * @param params	list of executable parameters.
 */
void sim_frontend_main(list params);

#endif