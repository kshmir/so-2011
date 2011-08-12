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


void catcher(int sig);
int newsockfd;

int main()
{
	int sockfd;
	char* c;
	int open=1;
	/* The local server port */
	struct sockaddr_in server = {AF_INET,SOCKETNUMBER,INADDR_ANY};
	static struct sigaction act;

	act.sa_handler=catcher;
	sigfillset(&(act.sa_mask));
	sigaction(SIGPIPE,&act,NULL);

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

	if(listen(sockfd,5)==-1)
	{
		perror("listen call failed");
		exit(1);
	}

	for(;open;)
	{
		printf("hola");
		/* accept a connection */
		if((newsockfd = accept(sockfd,NULL,NULL))==-1)
		{
			perror("accept call failed");
			exit(1);
		}

		/* spawn a child to deal with the connection */
		if(fork()!=0)
		{
			int i=0,l=DEF_RSV_SIZE;
			char ch;
			c=calloc(l,sizeof(char));
			while(recv(newsockfd,&ch,1,0)>0)
			{
				if(i>l)
				{
					l=1.2*l;
					c=realloc(c,l);
				}
				if(ch=='\0')
				{
					c[i++]=ch;
					c=realloc(c,i);
					printf("Se recibio: %s",c);
					send(newsockfd,c,i,0);
					i=0;
					l=DEF_RSV_SIZE;
					c=calloc(l,sizeof(char));
				}else
					c[i++]=ch;
			}
			//				send(newsockfd,&ch,1,0);
			/* when client is no longer sending information the socket
			 * can be closed and the child process terminated */
			close(newsockfd);
			exit(0);
		}
		/* parent doen't need the newsockfd */
		close(newsockfd);
	}
}

void msgRecieved(){}

void catcher(int sig)
{
	close(newsockfd);
	exit(0);
}
