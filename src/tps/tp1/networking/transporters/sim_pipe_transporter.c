#include "sim_pipe_transporter.h"

#include <unistd.h>
#include <fcntl.h>



struct sim_pipe_transporter {
	cstring				write_fifo;	
	cstring				read_fifo;
	
	int					write_ptr;
	int					read_ptr;
	
	transporter_mode	mode;
	
	int					client;
};

static cstring create_fifo_name(int target_id) {
	cstring fifo_name = cstring_copy("./tmp/fifo_");
	fifo_name = cstring_write(fifo_name, cstring_fromInt(target_id));
	return fifo_name;
}

static sim_pipe_transporter create_pipe_transporter(cstring write_fifo, cstring read_fifo, int client, transporter_mode mode) {
	sim_pipe_transporter pipe = (sim_pipe_transporter) malloc(sizeof(struct sim_pipe_transporter));
	pipe->write_fifo = write_fifo;
	pipe->read_fifo = read_fifo;	
	pipe->client = client;
	pipe->mode = mode;
	if (client == 1) {
		if (mode == MODE_WRITE || mode == MODE_READWRITE) {
			pipe->write_ptr = open(write_fifo, O_WRONLY);
		}
		if (mode == MODE_READ || mode == MODE_READWRITE) { 
			pipe->read_ptr  = open(read_fifo, O_RDONLY);
		}
	}
	else {
		if (mode == MODE_READ || mode == MODE_READWRITE) {
			pipe->read_ptr  = open(read_fifo, O_RDONLY);
		}
		if (mode == MODE_WRITE || mode == MODE_READWRITE) {
			pipe->write_ptr = open(write_fifo, O_WRONLY);
		}
	}
}

sim_pipe_transporter sim_pipe_transporter_init_client(int server_id, int client_id) {
	cstring write_fifo = create_fifo_name(server_id);
	cstring read_fifo = create_fifo_name(client_id);
	
	return create_pipe_transporter(write_fifo, read_fifo, 1, MODE_READWRITE);
}

sim_pipe_transporter sim_pipe_transporter_init_server(int server_id, int client_id, transporter_mode mode) {
	cstring write_fifo = NULL;
	cstring read_fifo = NULL;
	
	if (mode == MODE_WRITE || mode == MODE_READWRITE) {
		write_fifo = create_fifo_name(client_id);
		mkfifo(write_fifo, 0666);
	}
	if (mode == MODE_READ || mode == MODE_READWRITE) {
		read_fifo  = create_fifo_name(server_id);
		mkfifo(read_fifo, 0666);
	}
	
	return create_pipe_transporter(write_fifo, read_fifo, 0, mode);
}


void sim_pipe_transporter_write(sim_pipe_transporter t, cstring data) {
	if (t->mode == MODE_WRITE || t->mode == MODE_READWRITE) { 
		write(t->write_ptr, data, cstring_len(data) + 1);
	}
}

cstring sim_pipe_transporter_listen(sim_pipe_transporter t, int * extra_data) {
	
	if (t->mode == MODE_READ || t->mode == MODE_READWRITE) {
		cstring str = cstring_init(64);
		read(t->read_ptr, str, 64);
		* extra_data = 64;
		return str;
	}
	else{
		return "";
	}
}

void sim_pipe_transporter_free(sim_pipe_transporter transp) {
	
}