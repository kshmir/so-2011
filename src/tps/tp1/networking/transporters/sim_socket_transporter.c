#include "sim_socket_transporter.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>

#define SOCKET_READ_SIZE 32

struct sim_socket_transporter {
	int					server_id;
	int					client_id;
	
	int					server_fd;
	int					client_fd;
	
	int					is_client;
	
	int					listen_fd;
	int					write_fd;
	
	pthread_t		*	startup_thread;
	
	struct sockaddr_un 	server_add;
	struct sockaddr_un 	client_add;
};

static void * init_client_async(sim_socket_transporter t) {

	return NULL;
}


sim_socket_transporter sim_socket_transporter_init_client(int server_id, int client_id){
	
	sim_socket_transporter t = (sim_socket_transporter) calloc(sizeof(struct sim_socket_transporter), 1);

	t->client_id = client_id;
	t->server_id = server_id;
	t->is_client = 1;
	
//	t->startup_thread		= (pthread_t *) malloc(sizeof(pthread_t));
//	pthread_create(t->startup_thread, NULL, (void_p) init_client_async, (void_p) t);
	t->server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	int server_len = sizeof(struct sockaddr_un);
	
	t->server_add.sun_family = AF_UNIX;
	cstring path = cstring_copy("./tmp/sck_id_");
	path = cstring_write(path, cstring_fromInt(t->client_id));
	strcpy(t->server_add.sun_path, path);
	
	if (connect(t->server_fd, (void_p)&t->server_add, server_len) == -1) {
		perror("connect");	
	}
	
	t->listen_fd = t->server_fd;
	t->write_fd = t->server_fd;	

}

static void * init_server_async(sim_socket_transporter t) {
	

	return NULL;
}

sim_socket_transporter sim_socket_transporter_init_server(int server_id, int client_id){
	
	sim_socket_transporter t = (sim_socket_transporter) calloc(sizeof(struct sim_socket_transporter), 1);

	t->client_id = client_id;
	t->server_id = server_id;
	
//	t->startup_thread		= (pthread_t *) malloc(sizeof(pthread_t));
//	pthread_create(t->startup_thread, NULL, (void_p) init_server_async, (void_p) t);
	
	t->server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	t->server_add.sun_family = AF_UNIX;
	cstring path = cstring_copy("./tmp/sck_id_");
	path = cstring_write(path, cstring_fromInt(t->client_id));
	strcpy(t->server_add.sun_path, path);
	
	
	int server_len = sizeof(struct sockaddr_un);
	int client_len = sizeof(struct sockaddr_un);
	
	// 2
	if (bind(t->server_fd, (void_p)  &t->server_add, server_len) == -1) {
		perror("bind");
		exit(1);
	}
	
	// 3
	if (listen(t->server_fd, 5) == -1) {
		perror("listen");
		exit(1);
	}
	
	
	// 4
	if ((t->client_fd = accept(t->server_fd,(void_p)&t->client_add,&client_len)) == -1) {
		perror("accept");	
	}
	
	
	t->listen_fd = t->client_fd;
	t->write_fd = t->client_fd;
	return t;
}


void sim_socket_transporter_write(sim_socket_transporter t, cstring data){
	if (write(t->write_fd, data, cstring_len(data) + 1) == -1) {
	//		perror("write");	
	}
	
}

cstring sim_socket_transporter_listen(sim_socket_transporter t, int * len){
	cstring data = cstring_init(SOCKET_READ_SIZE + 1);
	if (read(t->listen_fd, data, SOCKET_READ_SIZE) == -1) {
	//	printf("I am %d\n", t->is_client);
	}
	* len = SOCKET_READ_SIZE; 

	return data;
}

void sim_socket_transporter_free(sim_socket_transporter transp){

	close(transp->server_fd);
	close(transp->client_fd);
	if (!transp->is_client) {
		cstring path = cstring_copy("./tmp/sck_id_");
		path = cstring_write(path, cstring_fromInt(transp->client_id));
		if (unlink(path) == -1){
			perror("unlink");
		}
	}

}
