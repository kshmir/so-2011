/*
 *  pipesServerExample.c
 *  
 *
 *  Created by Agustin Marseillan on 8/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "pipesServerExample.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>

#define MSGSIZE 6

char *msg1 = "hello";
char *msg2 = "bye!!";

void parent(int [3][2]);
int chilf(int []);
void fatal(char *msg);

main(){
	int pip[3][2];
	int i;
	
	//create three comunication pipes, and spawn three children
	for(i = 0; i<3; i++){
		if (pipe(pip[i]) == -1)
			fatal("pipe call");
		switch (fork()) {
			case -1:		//fork error
				fatal("fork call");
			case 0:
				child(pip[i]);
		}
	}
	parent(pip);
	
	exit(0);
}



//parent sits listening on all three pipes
void parent(int p[3][2])
{
	char buf[MSGSIZE], ch;
	fd_set set, master;
	int i;
	
	//close unwanted write file descriptors
	for(i = 0; i<3; i++)
		close(p[i][1]);
	
	//set the bit masks for the select system call
	FD_ZERO(&master);
	FD_SET(0, &master);
	
	for(i=0; i<3; i++)
		FD_SET(p[i][0], &master);
	
	//select is called with no timeout,
	//it will block untill an event occurs
	while (set = master, select(p[2][0]+1, &set, NULL, NULL, NULL) > 0 ) {
		//info from standart input
		if (FD_ISSET(0, &set)) {
			printf("From standard input...");
			read(0, &ch, 1);
			printf("%c\n", ch);
		}
		
		for (i=0; i<3; i++) {
			if (FD_ISSET(p[i][0], &set)) {
				if (read(p[i][0], buf, MSGSIZE) > 0) {
					sleep(1);
					printf("Message from child%d\n", i);
					printf("MSG=%s\n", buf);
				}
			}
		}
		//the server will return to the main program if all its
		//children have died
					
		if(waitpid(-1,NULL,WNOHANG) == -1)
					return;
	}
}

int child(int p[2])
{
	int count;
	
	close(p[0]);
	
	for (count = 0; count < 2; count++) {
		write(p[1], msg1, MSGSIZE);
		sleep(getpid()%4);
	}
	
	//send final message
	
	write(p[1], msg2, MSGSIZE);
	exit(0);
}

void fatal(char *msg){
	printf("a");
	printf("%s\n",msg);
	exit(1);
}