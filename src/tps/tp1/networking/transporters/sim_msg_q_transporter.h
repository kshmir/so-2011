/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------------------------------------------------------------
 * Message Queue Transporter Implementation.
 * The following implementation complies with the functions sim_transporter requires to work.
 */

#ifndef _SIM_MSG_Q_TRANSPORTER_H_
#define _SIM_MSG_Q_TRANSPORTER_H_

#include "../../sim_common_reqs.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MSGSZ    8 
#define MSGSEG   2048 
#define MSGMAX   (MSGSZ*MSGSEG) 
#define MSGMNB   2048 
#define MSGMNI   40 
#define MSGTQL   40 
#define MSGQ_KEY 2204

typedef struct sim_msg_q_transporter * sim_msg_q_transporter;

/**
	Starts a client
 */
sim_msg_q_transporter sim_msg_q_transporter_init_client(int server_id, int client_id);

/**
	Starts a server
 */
sim_msg_q_transporter sim_msg_q_transporter_init_server(int server_id, int client_id);

/**
	Writes through the transporter.
 */
void sim_msg_q_transporter_write(sim_msg_q_transporter t, cstring data);

/**
	Listens to the transporter.
 */
cstring sim_msg_q_transporter_listen(sim_msg_q_transporter t, int * extra_data);


/**
	Frees the transporter.
 */
void sim_msg_q_transporter_free(sim_msg_q_transporter transp);

#endif
