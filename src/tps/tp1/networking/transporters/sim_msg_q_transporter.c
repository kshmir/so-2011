#include "sim_msg_q_transporter.h"

	struct my_msgbuf buf;
	int msqid;
	key_t key;
	key = MSGQ_KEY;
	int clientId;

sim_msg_q_transporter sim_msg_q_transporter_init_client(int server_id, int client_id){

}

sim_msg_q_transporter sim_msg_q_transporter_init_server(int client_id, int server_id){

}

void sim_msg_q_transporter_write(sim_msg_q_transporter t, cstring data);

cstring sim_msg_q_transporter_listen(sim_msg_q_transporter t, int * extra_data){
			if (msgrcv(msqid, &buf, sizeof(buf.mtext), clientId, 0) == -1) {
				perror("msg not rcv");
				exit(1); t->read_ptr
			}
}

void sim_msg_q_transporter_free(sim_msg_q_transporter transp);

