#include "sim_socket_transporter.h"


static map sockfd_id=NULL;

sim_socket_transporter sim_socket_transporter_init_client(int server_id, int client_id){
	sim_socket_transporter t= malloc(sizeof (struct sim_socket_transporter));
	struct sockaddr_in s = {AF_INET,SOCKETNUMBER};
	struct sockaddr_in cli_addr;
	int clilen;
	int sockfd=*((int*)map_get(sockfd_id,&server_id));
	clilen=sizeof(cli_addr);
	int* cl=malloc(sizeof (int));
	int* newsockfd=malloc(sizeof (int));
	if(sockfd==NULL){
		perror("Invalid server Id");
		exit(1);
	}
	/* convert and store the server's IP address */
	s.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	/* Set up the transport end point */

	if((t->socketfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket call failed");
		exit(1);
	}
	/* connect the socket to the server's address */

	if(connect(t->socketfd,(struct sockaddr *)&s,SIZE)==-1)
	{
		perror("connect call failed");
		exit(1);
	}

	newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
	{
		perror("ERROR on accept");
		exit(1);
	}
	map_set(sockfd_id,cl,newsockfd);
	t->client=client_id;
	t->server=server_id;
	t->socketfd=sockfd;
	return t;
}

sim_socket_transporter sim_socket_transporter_init_server(int client_id, int server_id){
	sim_socket_transporter t= malloc(sizeof (struct sim_socket_transporter));
	if(sockfd_id==NULL){
		int sockfd;
		struct sockaddr_in cli_addr;
		struct sockaddr_in server = {AF_INET,SOCKETNUMBER,INADDR_ANY};

		sockfd_id=map_init(int_comparer, int_cloner);
		/* Set up the transport end point */

		if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
		{
			perror("socket call failed");
			exit(1);
		}
		/* bind address to the end point */

		if(bind(sockfd,(struct sockaddr *)&server,SIZE)==-1)
		{
			perror("bind call failed");
			exit(1);
		}

		/* start listening for incoming connections */

		if(listen(sockfd,WAITING_CONNECTIONS)==-1)
		{
			perror("listen call failed");
			exit(1);
		}

		map_set(sockfd_id,&server_id,&sockfd);
	}
	t->client=client_id;
	t->server=server_id;
	t->socket_number=NULL;
	t->socketfd=NULL;
	return t;
}

void sim_socket_transporter_write(sim_socket_transporter t, cstring data){
	int l,sockfd;
	l=cstring_len(data);
	if(t->socketfd==NULL)
		sockfd=*((int*)map_get(sockfd_id,&(t->server)));
	else
		sockfd=t->socketfd;
	write(sockfd,(char*)data,l);
}

cstring sim_socket_transporter_listen(sim_socket_transporter t, int * extra_data){
	int sockfd;
	if(t->socketfd==NULL)
		sockfd=*((int*)map_get(sockfd_id,&(t->server)));
	else
		sockfd=t->socketfd;
	char* ret;
	int max=DEF_RSV_SIZE;
	int i=0;
	ret=(char*)calloc(sizeof(char),max);
	ret[0]=1;
	int flag=1;
	while(flag){
		if(recv(sockfd,&ret[i++],1,0)<=0)
			flag=0;
		else{
			if(i+1>max){
				max=max*1.2;
				ret=(char*)realloc(ret,max);
			}
			if(ret[i-1]==0)
				flag=0;
		}
	}
	*extra_data=i+1;
	ret=(char*)realloc(ret,(i+1)*sizeof(char));
	return ret;

}

void sim_socket_transporter_free(sim_socket_transporter transp){
	if(transp->socketfd==NULL){
		list a=map_values(sockfd_id);
		foreach(int, t, a) {
			close(t);
		}
		list_free(a);
		map_free(sockfd_id);
	}else
		close(transp->socketfd);
	free(transp);
}
