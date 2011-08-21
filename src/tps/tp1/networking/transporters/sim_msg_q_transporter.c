#include "sim_msg_q_transporter.h"


sim_msg_q_transporter sim_msg_q_transporter_init_client(int server_id, int client_id){
	sim_msg_q_transporter t = malloc(sizeof(struct sim_msg_q_transporter));
	t->key=MSGQ_KEY;
	if ((t->msgq_id = msgget(t->key, 0644 )) == -1) { /* connect to the queue */
		perror("msgget");
		return NULL;
	}
	t->client=client_id;
	t->server=server_id;
	t->type=ISCLIENT;
	t->write_buf.mtype=t->server;
	t->read_buf.mtype=t->client;
	return t;
}

sim_msg_q_transporter sim_msg_q_transporter_init_server(int client_id, int server_id){
	sim_msg_q_transporter t = malloc(sizeof(struct sim_msg_q_transporter));
	t->key=MSGQ_KEY;
	if (client_id>255){
		perror("Client is > 255");
		return NULL;
	}

	if ((t->msgq_id = msgget(t->key, 0644| IPC_CREAT)) == -1) { /* connect to the queue */
		perror("msgget");
		return NULL;
	}
	t->client=client_id;
	t->server=server_id;
	t->type=ISSERVER;
	t->write_buf.mtype=t->client;
	t->read_buf.mtype=t->server;
	return t;
}

void sim_msg_q_transporter_write(sim_msg_q_transporter t, cstring data){
	int len = strlen(data);
	if(len>=MSGQ_MSG_SIZE-1)
		perror("Message was too big");
	if(t->type==ISCLIENT){
		strcpy(&(t->write_buf.mtext[1]),data);
		t->write_buf.mtext[0]=(char)t->client;
		len++;
	}else
		strcpy(t->write_buf.mtext,data);
	if (msgsnd(t->msgq_id, &(t->write_buf), len+1, 0) == -1) /* +1 for '\0' */
		perror("Message could not be send");
}

cstring sim_msg_q_transporter_listen(sim_msg_q_transporter t, int * extra_data){
	if (msgrcv(t->msgq_id, &(t->read_buf), MSGQ_MSG_SIZE, t->read_buf.mtype, 0) == -1) {
		perror("Message could not be received");
	}
	if(t->type==ISSERVER){
		*extra_data=(int)t->read_buf.mtext[0];
		return &(t->read_buf.mtext[1]);
	}
	*extra_data=NULL;
	return t->read_buf.mtext;
}

void sim_msg_q_transporter_free(sim_msg_q_transporter transp){
	if (msgctl(transp->msgq_id, IPC_RMID, NULL) == -1) {
		perror("There was a problem while closing the Message Queue");
	}
	free(transp);
}

