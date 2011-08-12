#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE sizeof(struct sockaddr_in)
#define SOCKETNUMBER 5115
#define DEF_RSV_SIZE 51

size_t recieveMsg(int sockfd, char* ret);
int sendMsg(int sockfd,char* msg);

int main()
{
	int sockfd;
	char c,rc;
	char* s=malloc(50);
	struct sockaddr_in server = {AF_INET,SOCKETNUMBER};

	/* convert and store the server's IP address */
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* Set up the transport end point */

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket call failed");
		exit(1);
	}
	/* connect the socket to the server's address */

	if(connect(sockfd,(struct sockaddr *)&server,SIZE)==-1)
	{
		perror("connect call failed");
		exit(1);
	}

	/* send and recieve information with the server */

	for(rc='\n';;)
	{
		printf("Input a string\n");
		scanf("%49s",s);

		if(sendMsg(sockfd,s)>0)
		{
			printf("enviado",rc);
			s[0]=0;
			int a=recieveMsg(sockfd,s);
			printf("%s  \n",s);
		}
		else
		{
			printf("server out\n");
			close(sockfd);
			exit(1);
		}
	}
}

int sendMsg(int sockfd,char* msg)
{
	size_t s=strlen(msg);
	printf("Mensaje enviado: %s",msg);
	return send(sockfd,msg,s,0);

}

size_t recieveMsg(int sockfd, char* ret)
{
	char rc=1;
	int max=DEF_RSV_SIZE;
	int i=0;
	ret=calloc(sizeof(char),max);
	while(rc!=0){
		recv(sockfd,&rc,1,0);
		if(i>max){
			max=max*1.2;
			ret=realloc(ret,max);
		}
		ret[i++]=rc;
	}
	ret[i]=0;
	ret=realloc(ret,i+1);
	return i;
}
