/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------------------------------------------------------------
 * Shared Memory Implementation
 * Read the .c file for details on the implementation
 */

#include "../../sim_common_reqs.h"

#ifndef _SIM_SMEM_TRANSPORTER_H_
#define _SIM_SMEM_TRANSPORTER_H_

typedef struct sim_smem_transporter sim_smem_transporter;

/**
 Init the shared memory.
 */
sim_smem_transporter * sim_smem_transporter_init(int server_id, int client_id, int is_server);

/**
 Write to the shared memoery.
 */
void sim_smem_transporter_write(sim_smem_transporter * t, cstring data);

/**
 Listen from the shared memory
 */
cstring sim_smem_transporter_listen(sim_smem_transporter * t, int * message_len);

/**
 Free up the shmems
 */
void sim_smem_transporter_free(sim_smem_transporter * transp);

#endif