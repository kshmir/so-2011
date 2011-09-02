#include "sim_socket_transporter.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>

#define SOCKET_READ_SIZE 64

static map server_descriptors = NULL;

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


sim_socket_transporter sim_socket_transporter_init_client(int server_id, int client_id){

	sim_socket_transporter t = (sim_socket_transporter) calloc(sizeof(struct sim_socket_transporter), 1);

	t->client_id = client_id;
	t->server_id = server_id;
	t->is_client = 1;

	t->server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	int server_len = sizeof(struct sockaddr_un);

	t->server_add.sun_family = AF_UNIX;
	cstring path = cstring_copy("./tmp/sck_id_");
	path = cstring_write(path, cstring_fromInt(t->client_id));
	strcpy(t->server_add.sun_path, path);


	while (connect(t->server_fd, (void_p)&t->server_add, server_len) == -1) {
		IPCSDebug(SOCK_DEBUG,"Couldn't connect to server: %s\n", t->server_add.sun_path);
	}

	t->listen_fd = t->server_fd;
	t->write_fd = t->server_fd;	

	return t;
}

sim_socket_transporter sim_socket_transporter_init_server(int server_id, int client_id){

	sim_socket_transporter t = (sim_socket_transporter) calloc(sizeof(struct sim_socket_transporter), 1);

	t->client_id = client_id;
	t->server_id = server_id;

	if (server_descriptors == NULL) {
		server_descriptors = map_init(int_comparer, NULL);
	}

	//	t->startup_thread		= (pthread_t *) malloc(sizeof(pthread_t));
	//	pthread_create(t->startup_thread, NULL, (void_p) init_server_async, (void_p) t);




	int server_len = sizeof(struct sockaddr_un);
	int client_len = sizeof(struct sockaddr_un);


	//	if (map_get(server_descriptors, &t->server_id) == NULL) {

	t->server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

	t->server_add.sun_family = AF_UNIX;
	cstring path = cstring_copy("./tmp/sck_id_");
	path = cstring_write(path, cstring_fromInt(t->client_id));
	strcpy(t->server_add.sun_path, path);

	if (bind(t->server_fd, (void_p)  &t->server_add, server_len) == -1) {
		IPCSDebug(SOCK_DEBUG,"Couldn't bind address %s\n", t->server_add.sun_path);
		exit(1);
	}
	int * _sid = (int*) malloc(sizeof(int));
	int * _sfd = (int*) malloc(sizeof(int));
	* _sid = t->server_id;
	* _sfd = t->server_fd;
	map_set(server_descriptors, _sid, _sfd);
	//	} else {
	//		t->server_fd = * (int*) map_get(server_descriptors, &t->server_id);
	//	}




	// 3
	if (listen(t->server_fd, 5) == -1) {
		IPCSDebug(SOCK_DEBUG,"Error while listening by server_fd:%d\n",t->server_fd);
		exit(1);
	}


	// Este codigo debería pasar a un thread que alimente todo.
	// Cola de mensajes y wow, estamos andando
	// 4
	if ((t->client_fd = accept(t->server_fd,(void_p)&t->client_add,&client_len)) == -1) {
		IPCSDebug(SOCK_DEBUG,"Error when accepting a connection by server_fd:%d\n",t->server_fd);
	}

	t->listen_fd = t->client_fd;
	t->write_fd = t->client_fd;
	return t;
}


void sim_socket_transporter_write(sim_socket_transporter t, cstring data){

	if (write(t->write_fd, data, cstring_len(data) + 1) == -1) {
		IPCSDebug(SOCK_DEBUG&WRITE,"Error while writting by write_fd:%d\n",t->write_fd);
	}else
		IPCSDebug(SOCK_DEBUG&READ,"SOCK sent: %s",data);
}


cstring sim_socket_transporter_listen(sim_socket_transporter t, int * len){
	cstring data = cstring_init(SOCKET_READ_SIZE + 1);
	if (read(t->listen_fd, data, SOCKET_READ_SIZE) == -1) {
		IPCSDebug(SOCK_DEBUG&READ,"I am %d (client=1, server=0), listen_fd: %d\n", t->is_client,t->listen_fd);
	}else
		IPCSDebug(SOCK_DEBUG&READ,"SOCK received: %s",data);
	* len = SOCKET_READ_SIZE; 

	return data;
}

void sim_socket_transporter_free(sim_socket_transporter transp){

	close(transp->server_fd);
	close(transp->client_fd);
	if (!transp->is_client) {
		if (map_get(server_descriptors, &transp->server_id) != NULL) {
			map_remove(server_descriptors, &transp->server_id);
		}
	}
	cstring path = cstring_copy("./tmp/sck_id_");
	path = cstring_write(path, cstring_fromInt(transp->client_id));
	unlink(path);

}
