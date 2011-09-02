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

int sem_create_typed(char * type) {
	char path[50];
	sprintf(path,"./tmp/sem_typed_%s",type);
	
	int op = open(path,O_CREAT, 0666);
	close(op);	
	
	key_t k = ftok(path, (char) 0);	
	
	int sem = semget(k, 1, (IPC_CREAT | 0666)); 
	if (sem < 0) {
		perror("Semaphore fail!");
		return -1;
	}
	return sem;
}

int sem_create(int key) {
	char path[50];
	sprintf(path,"./tmp/sem_%d",key);
	
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

int sem_set_value(int sem, int val) {
	union semun {
        int val;                    /* valor para SETVAL */
        struct semid_ds *buf;       /* buffer para IPC_STAT, IPC_SET */
        unsigned short int *array;  /* array para GETALL, SETALL */
        struct seminfo *__buf;      /* buffer para IPC_INFO */
	} s;
	s.val = val;
	return semctl(sem, 0, SETVAL, s);
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

int sem_free(int sem, int key) {

	if (semctl(sem, 0, IPC_RMID, NULL) == -1) {
		//perror("ERROR: could not clean up semaphore\n");
		return -1;
	}
	char path[50];
	sprintf(path,"./tmp/sem_%d",key);
	unlink(path);
	return 1;
}

int sem_free_typed(int sem, char * type) {
	
	if (semctl(sem, 0, IPC_RMID, NULL) == -1) {
		//perror("ERROR: could not clean up semaphore\n");
		return -1;
	}
	char path[50];						
	sprintf(path,"./tmp/sem_typed_%s",type);
	unlink(path);
	return 1;
}