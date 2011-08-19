
#ifndef _SIM_msg_q_TRANSPORTER_H_
#define _SIM_msg_q_TRANSPORTER_H_

#include "../../sim_common_reqs.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define MSGQ_KEY 2204

struct my_msgbuf {
	long mtype;
	char mtext[200];
};

typedef struct sim_msg_q_transporter * sim_msg_q_transporter;


sim_msg_q_transporter sim_msg_q_transporter_init_client(int server_id, int client_id);

sim_msg_q_transporter sim_msg_q_transporter_init_server(int client_id, int server_id);

void sim_msg_q_transporter_write(sim_msg_q_transporter t, cstring data);

cstring sim_msg_q_transporter_listen(sim_msg_q_transporter t, int * extra_data);

void sim_msg_q_transporter_free(sim_msg_q_transporter transp);

#endif
