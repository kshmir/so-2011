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

void doprocessing (int sock);
void catcher(int sig);
int newsockfd;

int main()
{
	int sockfd;
	char* c;
	int open=1;
	/* The local server port */
	struct sockaddr_in cli_addr;
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

	int clilen = sizeof(cli_addr);
	while (1)
	{
		newsockfd = accept(sockfd,
				(struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
		{
			perror("ERROR on accept");
			exit(1);
		}
		/* Create child process */
		int pid = fork();
		if (pid < 0)
		{
			perror("ERROR on fork");
			exit(1);
		}
		if (pid == 0)
		{
			/* This is the client process */
//			close(sockfd);
			doprocessing(newsockfd);
			exit(0);
		}
		else
		{
			close(newsockfd);
		}
	} /* end of while */
}

void msgRecieved(){}

void catcher(int sig)
{
	close(newsockfd);
	exit(0);
}

void doprocessing (int sock)
{
    int n;
    char buffer[256];

    bzero(buffer,256);

    n = read(sock,buffer,255);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("Here is the message: %s\n",buffer);
    n = write(sock,"I got your message",18);
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }
}

