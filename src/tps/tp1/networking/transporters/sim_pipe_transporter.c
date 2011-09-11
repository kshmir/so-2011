/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------------------------------------------------------------
 * FIFO implementation of transporter.
 */

#include "sim_pipe_transporter.h"

#include <unistd.h>
#include <fcntl.h>

#define PIPE_READ_SIZE (1024 * 8)

struct sim_pipe_transporter {
	cstring				write_fifo;			// Fifo to which the transporter writes.
	cstring				read_fifo;			// Fifo to which the transporter listens.

	int					write_ptr;			// File descriptor to which we write.
	int					read_ptr;			// File descriptor to which we read.
	
	int					client_id;
	int					server_id;

	cstring				read_data;
	
	transporter_mode	mode;				// Transporter mode.

	int					sem;
	int					client;				// Tells wether it's a client or not.
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
//	pipe->sem = sem_create_typed("pipes");
//	sem_set_value(pipe->sem, 1);
	// MacOSX doesn't seem to like going blocked, and it fails misserably.
	// So this solves it, making the right call when not on a Mach Kernel system (Mac OS, MacOSX, NextSTEP, etc).
	// We currently tested it only on Mac OSX 10.6.8, but in the worst case it'll only be porly performant.
#ifdef __MACH__
	if (client == 1) {
		if (mode == MODE_WRITE || mode == MODE_READWRITE) {
			do {
				pipe->write_ptr = open(write_fifo, O_WRONLY | O_NONBLOCK);
			} while(pipe->write_ptr == -1);

		}
		if (mode == MODE_READ || mode == MODE_READWRITE) { 
			do {
				pipe->read_ptr  = open(read_fifo, O_RDONLY);
			} while(pipe->read_ptr == -1);
		}
	}
	else {
		if (mode == MODE_READ || mode == MODE_READWRITE) {
			do {
				pipe->read_ptr  = open(read_fifo, O_RDONLY);
			} while(pipe->read_ptr == -1);
		}
		if (mode == MODE_WRITE || mode == MODE_READWRITE) {
			do {
				pipe->write_ptr = open(write_fifo, O_WRONLY | O_NONBLOCK);
			} while(pipe->write_ptr == -1);
		}
	}
#else
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
#endif


	return pipe;
}

sim_pipe_transporter sim_pipe_transporter_init_client(int server_id, int client_id) {
	cstring write_fifo = create_fifo_name(server_id);
	cstring read_fifo = create_fifo_name(client_id);

	sim_pipe_transporter t = create_pipe_transporter(write_fifo, read_fifo, 1, MODE_READWRITE);

	//t->sem = sem_create(client_id);
	//sem_up(t->sem, 1);
	
	t->server_id = server_id;
	t->client_id = client_id;
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

	sim_pipe_transporter t = create_pipe_transporter(write_fifo, read_fifo, 0, mode);

	//t->sem = sem_create(client_id);
//	sem_down(t->sem, 1);

	t->server_id = server_id;
	t->client_id = client_id;



	return t;
}


void sim_pipe_transporter_write(sim_pipe_transporter t, cstring data) {
	if (t->mode == MODE_WRITE || t->mode == MODE_READWRITE) { 
//		cprintf("I WRITE: %s\n", ROJO,data);
//		sem_down(t->sem, 1);
		if(write(t->write_ptr, data, cstring_len(data) + 1)==-1){
			//IPCSDebug(PIPE_DEBUG&WRITE,"Error while writting with write_ptr: %d",t->write_ptr);
		}else {
			//IPCSDebug(PIPE_DEBUG&WRITE,"PIPE sent: %s",data);
		}

	}
}

cstring sim_pipe_transporter_listen(sim_pipe_transporter t, int * length) {

	if (t->mode == MODE_READ || t->mode == MODE_READWRITE) {
		t->read_data = cstring_init(PIPE_READ_SIZE);
		if(read(t->read_ptr, t->read_data, PIPE_READ_SIZE)==-1){
			//IPCSDebug(PIPE_DEBUG&READ,"Error while reading with read_ptr: %d",t->read_ptr);
		}else
		//IPCSDebug(PIPE_DEBUG&READ,"PIPE rcvd: %s",str);
		* length = PIPE_READ_SIZE;
		
//		sem_up(t->sem, 1);	


		return t->read_data;
	}
	else{
		return "";
	}
}

void sim_pipe_transporter_free(sim_pipe_transporter t) {
	if (t->mode == MODE_READ || t->mode == MODE_READWRITE) {
		if(close(t->read_ptr)==-1)
			//IPCSDebug(PIPE_DEBUG&WRITE,"Error while closing fd: %d",t->read_ptr);
		unlink(t->read_fifo);
		free(t->read_fifo);
	}

	if (t->mode == MODE_WRITE || t->mode == MODE_READWRITE) { 
		if(close(t->write_ptr)==-1)
			//IPCSDebug(PIPE_DEBUG&WRITE,"Error while closing fd: %d",t->write_ptr);
		unlink(t->write_fifo);
		free(t->write_fifo);
	}
	free(t->read_data);

	
	free(t);
}
