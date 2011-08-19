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
	
	transporter_mode	mode;
	
	function			open;	
	function			write;	
	function			listen;	
	function			free;	
};	



void_p sim_transporter_listener(sim_transporter t) {
	int len, i;
	cstring builder = cstring_init(0);

	while(TRUE) {
		len = 0;
		char * data = (char *) t->listen(t->data, &len);
		i = 0;
		for (; i < len; i++) {
			builder = cstring_write_c(builder, data[i]);
			if (data[i] == 0) {
				if (cstring_len(builder) > 0) {
					queue_poll(t->messages, builder);
					pthread_cond_broadcast(t->listener_received);
					builder = cstring_init(0);
				}
			}
		}
		
	}

	
	return NULL;
}


void sim_transporter_dequeue(sim_transporter t) {
	queue_pull(t->messages);
}

cstring sim_transporter_listen(sim_transporter t) {
	int value_found = 0;
	cstring data = NULL;
	while(!value_found) {
		pthread_mutex_lock(t->listener_mutex);
		while (queue_empty(t->messages)) {
			pthread_cond_wait(t->listener_received, t->listener_mutex);
		}
		data = queue_peek(t->messages);
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
	pthread_t	*	thread		= (pthread_t *) malloc(sizeof(pthread_t));

	queue messages = queue_init();
	
	pthread_mutex_init(mutex, NULL);
	pthread_cond_init(received, NULL);


	tr->listener = thread;
	tr->messages = messages;
	tr->listener_mutex = mutex;
	tr->listener_received = received;
	
	return tr;
}


static void exec_process(process_type proc, connection_type type, int from_id, int to_id) {
	int id = 0;

	switch (proc) {
		case P_TESTER:		
			id = fork();
			if (id == 0) {
				execl("tp1_test_child", "tp1_test_child", cstring_fromInt(type),
					  cstring_fromInt(from_id), cstring_fromInt(to_id), NULL);
			} else {
				//int stat = 0;
				//waitpid(id,&stat,NULL);
			}

			break;
		default:
			break;
	}
}



// Used by the server process to build a process and open a connection to it.
sim_transporter sim_transporter_init(connection_type type, process_type proc, int from_id, int to_id, transporter_mode mode, int forks_child, int is_server) {
	
	sim_transporter t = sim_transporter_start();
	t->mode = mode;
	
	if (is_server && forks_child) {
		exec_process(proc, type, from_id, to_id);
	}
	
	switch (type) {
		case C_PIPE:
			if (is_server) {
				t->data = sim_pipe_transporter_init_server(from_id, to_id, mode);
			}
			else {
				t->data = sim_pipe_transporter_init_client(from_id, to_id);
			}

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
	if (mode != MODE_WRITE) {
		pthread_create(t->listener, NULL, (void_p) sim_transporter_listener, (void_p) t);
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