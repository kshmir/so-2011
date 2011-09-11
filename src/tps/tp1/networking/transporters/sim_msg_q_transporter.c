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



#include "sim_msg_q_transporter.h"

extern int msgq_id;

/**
 *	Structure used to send messages through the queue.
 */
struct msgq_buf {
	long mtype;					// Type of message to send, must be > 0.
	char mtext[MSGSEG];			// Text to send.
};

/**
 * Structure which extends the generic transporter.
 */
struct sim_msg_q_transporter {
	struct msgq_buf		read_buf;		// Buffer used to read.
	struct msgq_buf		write_buf;		// Buffer used to write.
	int					msgq_id;		// Message queue ID.
	int					server;			// Server ID.
	int					client;			// Client ID.
	int					mode;			// Connection mode.
	key_t				key;
};

sim_msg_q_transporter sim_msg_q_transporter_init_client(int server_id, int client_id){
	sim_msg_q_transporter t = malloc(sizeof(struct sim_msg_q_transporter));
	t->key=ftok("./tmp",'#');
	if ((t->msgq_id = msgget(t->key, 0600 | IPC_CREAT )) == -1) { /* connect to the queue */
		return NULL;
	}
	
	msgq_id = t->msgq_id;
	t->client			= client_id + 1;
	t->server			= server_id + 1;
	t->write_buf.mtype	= t->server;
	t->read_buf.mtype	= t->client;
	return t;
}

sim_msg_q_transporter sim_msg_q_transporter_init_server(int server_id, int client_id){
	sim_msg_q_transporter t = malloc(sizeof(struct sim_msg_q_transporter));
	t->key=ftok("./tmp",'#');

	t->msgq_id = msgget(t->key, 0600);
	
	if ((t->msgq_id = msgget(t->key, 0600 | IPC_CREAT)) == -1) { /* connect to the queue */
		return NULL;
	}
	
	msgq_id = t->msgq_id;
	t->client			= client_id + 1;
	t->server			= server_id + 1;
	t->write_buf.mtype	= t->client ;
	t->read_buf.mtype	= t->server;

	return t;
}

static int safe_strlen(char* s, int max_size) {
	int i = 0; 
	for (; i < max_size - 1 && s[i]; i++);
	return i + 1;
}

void sim_msg_q_transporter_write(sim_msg_q_transporter t, cstring data){
	cstring _data_buffer = data;
	int end = 0;
	int writes = 0;

	/**
		Messages here are sent as chunks, since the message queue might be limited.
	 */
	
	while(!end) {
		int block_len = sizeof(struct msgq_buf) - sizeof(long);
		int len = strlen(data);
		int index = 0;
		int i = 0;
		cstring d = cstring_copy_len(_data_buffer, block_len);
		_data_buffer += block_len;
		for (i = 0; i < block_len; i++) {
			t->write_buf.mtext[i] = 0;
		}
		for (i = 0; i < block_len; i++) {
			t->write_buf.mtext[i] = d[i];
			if (d[i] == 0) {
				end = 1;
				break;
			}
		}
		int attempts = 0;
		while (msgsnd(t->msgq_id, &t->write_buf, sizeof(struct msgq_buf) - sizeof(long), 0) == -1 && attempts < 100) /* +1 for '\0' */
		{
			// Sometimes it didn't just send it on OSX
			attempts++;
		}

		writes++;
		free(d);

	}

}

cstring sim_msg_q_transporter_listen(sim_msg_q_transporter t, int * extra_data){
	if (msgget(t->key, 0600) == -1)  {
		* extra_data = 0;
		return cstring_copy("");
	}
	
	int i= 0;
	for (; i < sizeof(struct msgq_buf) - sizeof(long); i++) {
		t->read_buf.mtext[i] = 0;
	}
	
	if (msgrcv(t->msgq_id, &(t->read_buf), sizeof(struct msgq_buf) - sizeof(long), t->read_buf.mtype, 0) == -1) {
		// perror("Not receiving msgqueue");
	}


	*extra_data = sizeof(struct msgq_buf) - sizeof(long);

	return cstring_copy(t->read_buf.mtext);
}

void sim_msg_q_transporter_free(sim_msg_q_transporter transp){
	msgctl(transp->msgq_id, IPC_RMID, NULL);	
	free(transp);
}

