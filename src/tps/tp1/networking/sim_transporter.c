#include "sim_transporter.h"
#include "transporters/sim_msg_q_transporter.h"
#include "transporters/sim_pipe_transporter.h"
#include "transporters/sim_smem_transporter.h"
#include "transporters/sim_socket_transporter.h"

#include <unistd.h>
#include <pthread.h>


struct sim_transporter {
	connection_type		type;  
	void_p				data;
	
	pthread_mutex_t	*	listener_mutex;
	pthread_cond_t	*	listener_received;
	
	pthread_t		*	listener;
	
	queue				messages;
	
	function			open;  
	function			write;
	function			listen;
	function			free; 
};	



void_p sim_transporter_listener(sim_transporter t) {
	int len, i, offset;
	cstring builder = cstring_init(0);
	while(1) {
		pthread_mutex_lock(t->listener_mutex);
		len = 0;
		char * data = (char *) t->listen(t->data, &len);
		printf("data received: %s", data);
		i = cstring_len(builder);
		cstring_expand(builder, len);
		for (; i < len; i++) {
			builder[i] = data[i + offset];
			if (builder[i] == 0) {
				queue_poll(t->messages, builder);
				pthread_cond_broadcast(t->listener_received);
				offset = i;
				i = 0;
				builder = cstring_init(len - offset);
			}
		}
		free(data);
		pthread_mutex_unlock(t->listener_mutex);
	}
	return NULL;
}

cstring sim_transporter_listen_request(sim_transporter sim, cstring request_match) {
	
}

cstring sim_transporter_listen(sim_transporter t) {
	int value_found = 0;
	cstring data = NULL;
	while(!value_found) {
		pthread_mutex_lock(t->listener_mutex);
		pthread_cond_wait(t->listener_received, t->listener_mutex);
		data = queue_pull(t->messages);
		if (data != NULL)
			value_found = 1;	
		pthread_mutex_unlock(t->listener_mutex);
	}
	return data;
}

static sim_transporter sim_transporter_start() {
	sim_transporter tr = (sim_transporter) malloc(sizeof(struct sim_transporter));
	
	pthread_mutex_t * mutex		= (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_cond_t * received	= (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_t	*	thread = (pthread_t *) malloc(sizeof(pthread_t));
	queue messages = queue_init();
	
	pthread_mutex_init(mutex, NULL);
	pthread_cond_init(received, NULL);
	

	tr->listener = thread;
	tr->messages = messages;
	tr->listener_mutex = mutex;
	tr->listener_received = received;
	
	return tr;
}

// Used by the client process to build a transporter to connect to it's server.
sim_transporter sim_transporter_init(connection_type type, int from_id, int to_id) {
	sim_transporter t = sim_transporter_start();
	switch (type) {
		case C_PIPE:
			t->data = sim_pipe_transporter_init_client(from_id, to_id);
			t->write = (function)	sim_pipe_transporter_write;
			t->listen = (function)	sim_pipe_transporter_listen;
			break;
		case C_SHARED_MEMORY:
			// bind functions to C_SHARED_MEMORY implementation.
			break;
		case C_SOCKETS:
			// bind functions to C_SOCKETS implementation.
			break;	
		case C_M_QUEUES:
			// bind functions to C_M_QUEUES implementation.
			break;	
		default:
			break;
	}
	pthread_create(t->listener, NULL, (void_p) sim_transporter_listener, (void_p) t);
	return t;
}


static void exec_process(process_type proc, connection_type type, int from_id, int to_id) {
	int id = 0;

	switch (proc) {
		case P_TESTER:		
			id = fork();
			if (id == 0) {
				execl("tp1_test_child", "tp1_test_child", cstring_fromInt(type),
					  cstring_fromInt(from_id), cstring_fromInt(to_id), NULL);
			}
			break;
		default:
			break;
	}
}



// Used by the server process to build a process and open a connection to it.
sim_transporter sim_transporter_fork(connection_type type, process_type proc, int from_id, int to_id) {
	
	sim_transporter t = sim_transporter_start();
	
	// Make fork/exec of the connection type given, and pass the parameters to start the connection.
	switch (type) {
		case C_PIPE:
			exec_process(proc, type, from_id, to_id);
			t->data = sim_pipe_transporter_init_server(from_id, to_id);
			t->write = (function)  sim_pipe_transporter_write;
			t->listen = (function) sim_pipe_transporter_listen;
			break;
		case C_SHARED_MEMORY:
			// bind functions to C_SHARED_MEMORY implementation.
			break;
		case C_SOCKETS:
			// bind functions to C_SOCKETS implementation.
			break;	
		case C_M_QUEUES:
			// bind functions to C_M_QUEUES implementation.
			break;	
		default:
			break;
	}
	return t;
}





void sim_transporter_write(sim_transporter sim, cstring message) {
	sim->write(sim->data, message);
}

void sim_transporter_free(sim_transporter sim) {
	sim->free(sim->data);
	free(sim);
}