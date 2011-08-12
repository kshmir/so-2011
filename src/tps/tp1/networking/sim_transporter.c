#include "sim_transporter.h"
#include "transporters/sim_msg_q_transporter.h"
#include "transporters/sim_pipe_transporter.h"
#include "transporters/sim_smem_transporter.h"
#include "transporters/sim_socket_transporter.h"

#include <unistd.h>


struct sim_transporter {
	connection_type type;  
	void_p			data;
	function		open;  
	function		write; 
	function		listen;
	function		close; 
	function		free; 
};	

// Used by the client process to build a transporter to connect to it's server.
sim_transporter sim_transporter_init(connection_type type, int from_id, int to_id) {
	
	// Now that you're done, connect back to the server and start the connection.
	switch (type) {
		case C_PIPE:
			// bind functions to C_PIPE implementation.
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
}

// Used by the server process to build a process and open a connection to it.
sim_transporter sim_transporter_fork(connection_type type, process_type proc, int from_id, int to_id) {

	
	
	// Make fork/exec of the connection type given, and pass the parameters to start the connection.
	switch (type) {
		case C_PIPE:
			// bind functions to C_PIPE implementation.
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
}

void sim_transporter_write(sim_transporter sim, cstring message) {
	sim->write(sim->data, message);
}

cstring sim_transporter_listen(sim_transporter sim) {
	sim->listen(sim->data);
}

void sim_transporter_free(sim_transporter sim) {
	sim->close(sim->data);
	free(sim);
}