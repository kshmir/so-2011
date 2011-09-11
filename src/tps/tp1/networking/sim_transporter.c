/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------------------------------------------------------------
 * sim_transporter interface.
 *		sim_transporter is the basic implementation of our IPC layer.
 *		It handles an inheritance pattern via pointers to function, the three basic pointers are
 *		write, listen and free. It is initialized in sim_transporter_init which also initializes 
 *		the current implementation.
 *      It can be set as READ, WRITE or READWRITE.
 *      It can also build another process by forking it.
 *      When writing, it just writes directly to the target it actually points.
 *		When reading, it does the following:
 *			It starts a neverending thread which is only stopped when freeing the transporter.
 *          The thread is used to "listen" to the other transporter end and push the data to the data queue.
 *          After it pushes something to the queue, the subscribed threads receive a call and are able to read 
 *			each message.
 *			This kind of implementation allows multiple threads to listen to one end at the same time.
 *          After a message is read, it must be dequeued if accepted.
 *		When freeing, it does the following:
 *			It makes a cancel call to the listener thread.
 *			After that call an async handler called sim_transporter_cleanup is executed and frees everything up,
 *          safely stopping the thread, which might otherwise be locked by the listener.
 * sim_transporter should only be used by:
 *		sim_server
 *      sim_client
 *      sim_message
 */

#include "sim_transporter.h"
#include "transporters/sim_msg_q_transporter.h"
#include "transporters/sim_pipe_transporter.h"
#include "transporters/sim_smem_transporter.h"
#include "transporters/sim_socket_transporter.h"
 
#include <unistd.h>
#include <pthread.h>
#include <signal.h>


int shm_lock = 0;


int					sck_override = 0;
queue				sck_override_queue = NULL;
pthread_mutex_t		sck_override_mutex;
pthread_cond_t		sck_override_received;

/**
 Data structure for the transporter.
 */
struct sim_transporter {
	connection_type		type;							// Type of connection, it decides which IPC should run.
	void_p				data;							// Contains the data of the inherited IPC handler.
	
	pthread_mutex_t	*	listener_mutex;					// Mutex for the distribuitor
	pthread_cond_t	*	listener_received;				// Cond used to unlock the subscribers
	
	pthread_t		*	listener;						// Thread which listens and pushes to the subscribers.
	
	transporter_mode	mode;							// Tells whether its RD, WR or RDWR
	
	int					client_id;						// Client ID - not actually used
	int					server_id;						// Server ID - not actually used
	
	function			write;							// Write handler
	function			listen;							// Listen handler
	function			_free;							// Free handler
	
	int					write_lock;
	int					read_lock;
	
	int					is_server;
	
	int					reads;
	
	queue				messages;						// Queue of received messages.
};	

/**
 Accesor for client_id, it is used in upper levels of the IPC implementation, like server or client.
 @param t The transporter
 @return The client_id of the transporter.
 */
int sim_transporter_client_id(sim_transporter t) {
	return t->client_id;
}

/**
 Accesor for server_id, it is used in upper levels of the IPC implementation, like server or client.
 @param t The transporter
 @return The server_id of the transporter.
 */
int sim_transporter_server_id(sim_transporter t) {
	return t->server_id;
}

/**
 Cleans up a transporter, after stopping the listener, or directly if it's writeonly.
 */
static void sim_transporter_cleanup(sim_transporter t) {
	// We don't use it since we started using free.
}




/**
 Listens constantly to the underlying IPC, builds cstrings out of it, and pushes them to the main queue.
 */
static void_p sim_transporter_listener(sim_transporter t) {
	int len, i, oldtype;
	cstring builder = cstring_init(0);
	
	pthread_cleanup_push((void_p)sim_transporter_cleanup, t);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);


	while(TRUE) {

		// This loops enqueues as many messages as built by the char * array.
		// This helps us to only care about sending zero ending strings as messages.
		len = 0;

		char * data = (char *) t->listen(t->data, &len);

		pthread_cond_broadcast(t->listener_received);
		if (sck_override == 1) {
			pthread_cond_broadcast(&sck_override_received);			
		}
		i = 0;
		

		
		if (data[0] == 0) {
			free(data);
			continue;
		}



		for (; len > 0 && i < len; i++) {			
			builder = cstring_write_c(builder, data[i]);
			
			if (data[i] == 0) {
				if (cstring_len(builder) > 0) {
					pthread_mutex_lock(t->listener_mutex);					

					if (t->is_server && sck_override) {
						pthread_mutex_lock(&sck_override_mutex);
						queue_poll(sck_override_queue, cstring_copy(builder));
						pthread_cond_broadcast(&sck_override_received);
						pthread_mutex_unlock(&sck_override_mutex);
					} else {
						queue_poll(t->messages, cstring_copy(builder));
						pthread_cond_broadcast(t->listener_received);
					}
									
					pthread_mutex_unlock(t->listener_mutex);		
					free(builder);
					builder = cstring_init(0);

					sem_up(t->write_lock, 1);
					break;
				}
				else {
					if (strlen(builder) == 0) {
						free(builder);
					}
					builder = cstring_init(0);
				}

			}
			
		}		

		free(data);

	}

	// The following is a macro for cancelling the thread
	pthread_cleanup_pop(0);
	pthread_exit(NULL);
}

/**
 Takes off the last message read from the queue.
 TWO subscribers shouldn't accept the same message at the same time.
 */
void sim_transporter_dequeue(sim_transporter t) {
	pthread_mutex_lock(t->listener_mutex);
	
	if (queue_size(t->messages) > 0) {
		free(queue_pull(t->messages));
		
		pthread_cond_broadcast(t->listener_received);
	}

	pthread_mutex_unlock(t->listener_mutex);
}



/**
 Listens and locks until there is something to read.
 It *** might *** always return the same string until another process has read it successfully. 
 It would be great to improve this.
 */
cstring sim_transporter_listen(sim_transporter t, cstring avoid) {
	int value_found = 0;
	cstring data = NULL;
	
	while(!value_found) {
		
		pthread_mutex_lock(t->listener_mutex);
		
		while (queue_empty(t->messages) || (avoid != NULL && cstring_compare(queue_peek(t->messages), avoid) == 0)) {
		
			pthread_cond_wait(t->listener_received, t->listener_mutex);
		}
		
		if (!queue_empty(t->messages)) {
			pthread_cond_broadcast(t->listener_received);
		}
		if (queue_peek(t->messages) != NULL) {
			data = cstring_copy(queue_peek(t->messages));
			value_found = 1;	
			t->reads++;
		}
		

		pthread_mutex_unlock(t->listener_mutex);
	}
	return data;
}

/**
 Starts a transporter, but makes no connections, only memory stuff.
 */
static sim_transporter sim_transporter_start() {
	sim_transporter tr = (sim_transporter) malloc(sizeof(struct sim_transporter));
	
	shm_lock = sem_create(240);
	sem_set_value(shm_lock, 1);
	
	pthread_mutex_t * mutex		= (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_cond_t * received	= (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_t	*	thread		= (pthread_t *) malloc(sizeof(pthread_t));
	
	queue messages = queue_init();
	
	pthread_mutex_init(mutex, NULL);
	pthread_cond_init(received, NULL);
	
	shm_lock = sem_create_typed("shm_lock");
	sem_set_value(shm_lock, 1);
	
	tr->write_lock = sem_create_valued(100999, 1);

	tr->read_lock = sem_create_valued(100998, 1);
	
	
	tr->listener = thread;
	tr->messages = queue_init();
	tr->listener_mutex = mutex;
	tr->listener_received = received;
	
	return tr;
}

/**
 Opens a new process and passes the corresponding params to it.
 */
static void exec_process(process_type proc, connection_type type, int from_id, int to_id) {
	int id = 0;
	int parent = getpid();
	switch (proc) {
		case P_TESTER:
			id = fork();
			if (id == 0) {
				execl("./tp1_test_child", "tp1_test_child", cstring_fromInt(type),
					  cstring_fromInt(from_id), cstring_fromInt(to_id), NULL);
			}
			break;
		case P_TESTER_SERVER:
			id = fork();
			if (id == 0) {
				execl("./tp1_test_server", "tp1_test_server", cstring_fromInt(type),
					  cstring_fromInt(from_id), cstring_fromInt(to_id), NULL);
			}
			break;
		case P_LEVEL:
			id = fork();
			if (id == 0) {
				setpgid(id, parent);
				execl("./tp1_level", "tp1_level", cstring_fromInt(type),
					  cstring_fromInt(from_id), cstring_fromInt(to_id), NULL);
			}
			break;
		case P_AIRLINE:
			id = fork();
			if (id == 0) {
				parent = getpgid(parent);
				setpgid(id, parent);
				execl("./tp1_airline", "tp1_airline", cstring_fromInt(type),
					  cstring_fromInt(from_id), cstring_fromInt(to_id), NULL);
			}
			break;
		default:
			break;
	}
}


/**
 Starts a new transporter and opens the connections.
 It can fork a process or start as a client. 
 @param type The type of IPC to use.
 @param proc The type of process to open.
 @param from_id The id from which the transporter is started
 @param to_id   The id to which the transporter is connected, it might create it if it forks_child
 @param mode    The mode with with the transporter operates, READ, READWRITE or WRITE.
 @param forks_child Tells wether the transporter forks a new child. It must be true and must have is_server as true.
 @param is_server It tells if it forks a server.
 @return The local endpoint of the transporter built, with the listener on if it's set to read.
 */
sim_transporter sim_transporter_init(connection_type type, 
									 process_type proc, 
									 int from_id, 
									 int to_id, 
									 transporter_mode mode, 
									 int forks_child, 
									 int is_server) {

	sim_transporter t = sim_transporter_start();
	t->mode = mode;
	t->reads = 0;
	t->is_server = is_server;
	
	if (is_server) {
		t->server_id = from_id;
		t->client_id = to_id;
		
	} else {
		t->client_id = from_id;
		t->server_id = to_id;
	}
	
	switch (type) {
		case C_PIPE:
			if (is_server && forks_child) {
				exec_process(proc, type, from_id, to_id);
			}
			if (is_server) {
				t->data = sim_pipe_transporter_init_server(from_id, to_id, mode);
			}
			else {
				t->data = sim_pipe_transporter_init_client(from_id, to_id);
			}

			t->write  = (function) sim_pipe_transporter_write;
			t->listen = (function) sim_pipe_transporter_listen;
			t->_free   = (function) sim_pipe_transporter_free;
			t->type = C_PIPE;
			break;
		case C_SHARED_MEMORY:
			if (is_server && forks_child) {
				exec_process(proc, type, from_id, to_id);
			}
			t->data = sim_smem_transporter_init(from_id, to_id, is_server);
			t->write = (function) sim_smem_transporter_write;
			t->listen = (function) sim_smem_transporter_listen;
			t->_free   = (function) sim_smem_transporter_free;
			t->type = C_SHARED_MEMORY;
			break;
		case C_SOCKETS:
			if (is_server && forks_child) {
				exec_process(proc, type, from_id, to_id);
			}
			if (is_server) {
				t->data = sim_socket_transporter_init_server(from_id, to_id);
			}
			else {
				t->data = sim_socket_transporter_init_client(from_id, to_id);
			}
			t->write  = (function) sim_socket_transporter_write;
			t->listen = (function) sim_socket_transporter_listen;
			t->_free   = (function) sim_socket_transporter_free;
			t->type = C_SOCKETS;
			break;
		case C_M_QUEUES:
			if (is_server) {
				t->data = sim_msg_q_transporter_init_server(from_id, to_id);
			}
			else {
				t->data = sim_msg_q_transporter_init_client(from_id, to_id);
			}
			t->write  = (function) sim_msg_q_transporter_write;
			t->listen = (function) sim_msg_q_transporter_listen;
			t->_free   = (function) sim_msg_q_transporter_free;
			t->type = C_M_QUEUES;
			if (is_server && forks_child) {
				exec_process(proc, type, from_id, to_id);
			}
			break;
		default:
			break;
	}
	if (mode != MODE_WRITE || type == C_SOCKETS) {
		pthread_create(t->listener, NULL, (void_p) sim_transporter_listener, (void_p) t);
		declare_thread(t->listener);
	}
	return t;
}

/**
 Writes a message to the current transporter.
 */
void sim_transporter_write(sim_transporter sim, cstring message) {
	sem_down(sim->write_lock, 1);
	sim->write(sim->data, message);
}

/**
 Free's up a transporter
 */
void sim_transporter_free(sim_transporter sim) {
	if (sim->mode != MODE_WRITE || sim->type == C_SOCKETS) {
		pthread_cancel(*sim->listener);
	} else {
		sim_transporter_cleanup(sim);
	}
	
}
