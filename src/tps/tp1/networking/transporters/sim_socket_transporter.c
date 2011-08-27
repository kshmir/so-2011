#include "sim_socket_transporter.h"


static map sockfd_id=NULL;
static int serverfd=0;

sim_socket_transporter sim_socket_transporter_init_client(int server_id, int client_id){
	sim_socket_transporter t= malloc(sizeof (struct sim_socket_transporter));
	struct sockaddr_in cli_addr;
	int clilen;
	int sockfd=-1;
	clilen=sizeof(cli_addr);
	int* cl=malloc(sizeof (int));
	*cl=client_id;

	struct sockaddr_in server = {AF_INET,SOCKETNUMBER};
	struct sockaddr_in client={AF_INET,INADDR_ANY,INADDR_ANY};
	/* convert and store the server's IP address */
	server.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))==-1)
	{
		perror("socket call failed");
		exit(1);
	}
	if(bind(sockfd,(struct sockaddr *)&client,SIZE)==-1)
	{
		perror("connect call failed");
		exit(1);
	}
	if(sockfd==-1){
		perror("Invalid server Id");
		exit(1);
	}

	t->client=client_id;
	t->server=server_id;
	t->socket_number=sockfd;
	t->socketfd=sockfd;
	t->client_sockaddr_in=client;
	t->server_sockaddr_in=server;
	char desc[3];
	desc[0]=0;
	desc[1]=(char)client_id;
	desc[2]=0;
	if(sendto(sockfd,desc,3,0,(struct sockaddr *)&server,SIZE)==-1)
		perror("error al autenticarse");
	printf("client: %d, server: %d, socketfd: %d",t->client,t->server,t->socketfd);
	return t;
}

sim_socket_transporter sim_socket_transporter_init_server(int client_id, int server_id){
	sim_socket_transporter t= malloc(sizeof (struct sim_socket_transporter));
	struct sockaddr_in cli_addr;
	if(sockfd_id==NULL){
		int* sockfd=malloc(sizeof(int));
		int* sid=malloc(sizeof(int));
		//		*sid=server_id;
		*sid=0;
		/* The local server port */
		struct sockaddr_in server = {AF_INET,SOCKETNUMBER,INADDR_ANY};

		sockfd_id=map_init(int_comparer, int_cloner);
		/* Set up the transport end point */

		if((*sockfd=socket(AF_INET,SOCK_DGRAM,0))==-1)
		{
			perror("socket call failed");
			exit(1);
		}
		/* bind address to the end point */

		if(bind(*sockfd,(struct sockaddr *)&server,SIZE)==-1)
		{
			perror("bind call failed");
			exit(1);
		}
		serverfd=*sockfd;

		printf("mksrv : %d\n",*sockfd);
		map_set(sockfd_id,sid,sockfd);
	}
	t->clilen = sizeof(cli_addr);
	t->client_len=sizeof(struct sockaddr_in);
	t->client=client_id;
	t->server=0;
	t->socket_number=-1;
	t->socketfd=-1;
	return t;
}

void sim_socket_transporter_write(sim_socket_transporter t, cstring data){
	int l,sockfd=0;
	if(t==NULL)
		return;
	l=cstring_len(data);
	if(t->socketfd==-1){
		sockfd=serverfd;
		if(map_get(sockfd_id,&t->client)==NULL)
			if(send(serverfd,data,l,0)==-1)
				perror("error al enviar 1");
		else{
			t->client_sockaddr_in=*((struct sockaddr_in*)map_get(sockfd_id,&t->client));
			if(sendto(serverfd,data,l,0,(struct sockaddr*)(&(t->client_sockaddr_in)),(socklen_t)t->client_len)==-1)
			perror("error al enviar 2");
		}
		printf("Soy server y escribo el dato: \"%s\" con el socket: %d\n",data,serverfd);
	}else{
		sockfd=t->socketfd;
		if(sendto(t->socketfd,data,l,0,(struct sockaddr*)(&(t->server_sockaddr_in)),(socklen_t)SIZE)==-1)
			perror("error al enviar 3");
		printf("Soy client y ");
	printf("escribo el dato: \"%s\" con el socket: %d\n",data,sockfd);
	}
}

cstring sim_socket_transporter_listen(sim_socket_transporter t, int * extra_data){
	int sockfd;
	if(t->socketfd==-1){
		sockfd=serverfd;
	}else{
		sockfd=t->socketfd;
		printf("\n client sockfd: %d\n",sockfd);
	}

	//	char ret;
	//	recv(sockfd,&ret,1,0);
	struct sockaddr_in cli_addr;
	int client_len=sizeof(struct sockaddr_in);
	struct sockaddr_in client;

	struct sockaddr_in* cli_addr_map=malloc(sizeof(struct sockaddr_in));

	int* cl_map_id=malloc(sizeof(int));
	char* ret;
	int max=DEF_RSV_SIZE;
	int i=0,aux;
	ret=(char*)calloc(sizeof(char),max);
	ret[0]=1;
	int flag=1;
	while(flag){
		printf("i: %d\n",i);
		if(t->socketfd==-1){
			aux=recvfrom(sockfd,&ret[i++],1,0,(struct sockaddr*)&client,(socklen_t *)&client_len);
		}else{
			aux=recv(sockfd,&ret[i++],1,0);
		}
		printf("\naux: %d, ret[%d]: %c\n",aux,i-1,ret[i-1]);
		if(aux<=0)
			flag=0;
		else{
			if(i+1>max){
				max=max*1.2;
				ret=(char*)realloc(ret,max);
			}
			if(ret[i-1]==0 && i!=1){
				if(i!=3 || t->socketfd!=-1 )
					flag=0;
				else{
					if(ret[2]==0){
						*cl_map_id=(int)ret[1];
						memcpy(cli_addr_map,&cli_addr,sizeof(struct sockaddr_in));
						map_set(sockfd_id,cl_map_id,cli_addr_map);
						printf("\nSe agrego cliente %d\n",*cl_map_id);
					}
					flag=0;
				}
			}
		}
//		printf("ret[%d]: %c",i-1,ret[i-1]);
	}
	*extra_data=i+1;
	//	ret=(char*)realloc(ret,(i+1)*sizeof(char));
	printf("ret: %s",ret);
	return ret;

}

void sim_socket_transporter_free(sim_socket_transporter transp){
	if(transp->socketfd==-1){
		list a=map_values(sockfd_id);
		foreach(struct sockaddr_in*, t, a) {
			free(t);
		}
		list_free(a);
		map_free(sockfd_id);
		close(serverfd);
	}else
		close(transp->socketfd);
	free(transp);
}
