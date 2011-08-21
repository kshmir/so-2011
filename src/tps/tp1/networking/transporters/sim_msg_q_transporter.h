
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

#define MSGQ_KEY 2204
#define MSGQ_MSG_SIZE 250
#define	ISSERVER	1
#define	ISCLIENT	0

struct msgq_buf {
	long mtype;
	char mtext[MSGQ_MSG_SIZE];
};
struct sim_msg_q_transporter {
	struct msgq_buf		read_buf;
	struct msgq_buf		write_buf;
	int					type;
	int					msgq_id;
	int					server;
	int					client;
	key_t				key;

};

typedef struct sim_msg_q_transporter * sim_msg_q_transporter;


sim_msg_q_transporter sim_msg_q_transporter_init_client(int server_id, int client_id);

sim_msg_q_transporter sim_msg_q_transporter_init_server(int client_id, int server_id);

void sim_msg_q_transporter_write(sim_msg_q_transporter t, cstring data);

cstring sim_msg_q_transporter_listen(sim_msg_q_transporter t, int * extra_data);

void sim_msg_q_transporter_free(sim_msg_q_transporter transp);

#endif
