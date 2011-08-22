#include "../../sim_common_reqs.h"

#ifndef _SIM_socket_TRANSPORTER_H_
#define _SIM_socket_TRANSPORTER_H_

#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE sizeof(struct sockaddr_in)
#define SOCKETNUMBER 5115
#define DEF_RSV_SIZE 51
#define	WAITING_CONNECTIONS	256
#define SERVER_ADDR	"127.0.0.1"


struct sim_socket_transporter{
		int					server;
		int					client;
		int					socket_number;
		int					socketfd;

	};
typedef struct sim_socket_transporter * sim_socket_transporter;


sim_socket_transporter sim_socket_transporter_init_client(int server_id, int client_id);

sim_socket_transporter sim_socket_transporter_init_server(int client_id, int server_id);

void sim_socket_transporter_write(sim_socket_transporter t, cstring data);

cstring sim_socket_transporter_listen(sim_socket_transporter t, int * extra_data);

void sim_socket_transporter_free(sim_socket_transporter transp);

#endif
