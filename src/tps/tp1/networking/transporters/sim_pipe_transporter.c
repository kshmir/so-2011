#include "sim_pipe_transporter.h"

#include <unistd.h>
#include <fcntl.h>

struct sim_pipe_transporter {
	cstring write_fifo;	
	cstring read_fifo;
	
	int		write_ptr;
	int		read_ptr;
	
};

static cstring create_fifo_name(int from_id, int to_id) {
	cstring fifo_name = cstring_copy("fifo_");
	cstring_write(fifo_name, cstring_fromInt(from_id));
	cstring_write(fifo_name, "_");
	cstring_write(fifo_name, cstring_fromInt(to_id));
	return fifo_name;
}

static sim_pipe_transporter create_pipe_transporter(cstring write_fifo, cstring read_fifo, int client) {
	sim_pipe_transporter pipe = (sim_pipe_transporter) malloc(sizeof(struct sim_pipe_transporter));
	pipe->write_fifo = write_fifo;
	pipe->read_fifo = read_fifo;	
	if (client == 1) {
		pipe->write_ptr = open(write_fifo, O_WRONLY);
		pipe->read_ptr  = open(read_fifo, O_RDONLY);
	}
	else {

		pipe->read_ptr  = open(read_fifo, O_RDONLY);
		pipe->write_ptr = open(write_fifo, O_WRONLY);
	}
}

sim_pipe_transporter sim_pipe_transporter_init_client(int server_id, int client_id) {
	cstring write_fifo = create_fifo_name(server_id, client_id);
	cstring read_fifo = create_fifo_name(client_id, server_id);
	
	return create_pipe_transporter(write_fifo, read_fifo, 1);
}

sim_pipe_transporter sim_pipe_transporter_init_server(int client_id, int server_id) {
	cstring write_fifo = create_fifo_name(server_id, client_id);
	cstring read_fifo = create_fifo_name(client_id, server_id);
	
	mkfifo(write_fifo, 0666);
	mkfifo(read_fifo, 0666);
	
	return create_pipe_transporter(write_fifo, read_fifo, 0);
}


void sim_pipe_transporter_write(sim_pipe_transporter t, cstring data) {
	printf("about to write on %s\n",t->write_fifo);
	write(t->write_ptr, data, cstring_len(data) + 1);
}

cstring sim_pipe_transporter_listen(sim_pipe_transporter t, int * extra_data) {
	cstring str = cstring_init(64);
	printf("about to read on %s\n",t->read_fifo);
	read(t->read_ptr, str, 64);
	* extra_data = 64;
	return str;
}

void sim_pipe_transporter_free(sim_pipe_transporter transp) {
	
}