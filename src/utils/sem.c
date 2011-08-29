/*
 *  sem.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 27/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include "sem.h"
#include <stdio.h>

int sem_create(int key) {
	char path[50];
	sprintf(path,"./tmp/sem%d",key);
	
	int op = open(path,O_CREAT, 0666);
	close(op);	
	
	key_t k = ftok(path, (char) (key));	
	int sem = semget(k, 1, (IPC_CREAT | 0666)); 
	if (sem < 0) {
		perror("Semaphore fail!");
		return -1;
	}
	return sem;
}

int sem_value(int sem) {
	return semctl(sem, 0, GETVAL, NULL);
}


int sem_up(int sem, int amount) {
	struct sembuf sops;
	sops.sem_num = 0;
	sops.sem_op = amount; /* semaphore operation */
	sops.sem_flg = SEM_UNDO;
	
	if (semop(sem, &sops, 1) == -1) {
	//	perror("Semaphore fail on UP");
		return -1;
	}
	return 1;
}

int sem_down(int sem, int amount) {
	sem_up(sem, -amount);
}

int sem_free(int sem) {
	if (semctl(sem, 0, IPC_RMID, NULL) == -1) {
		
		//perror("ERROR: could not clean up semaphore\n");
		return -1;
	}
	return 1;
}