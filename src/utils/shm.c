/*
 *  shm.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 27/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include "shm.h"

static int shm_id = 0;

void * shm_create(size_t s) {
	if (shm_id != 0) {
		return NULL;
	}
	const char * path = "./tmp/mem";
	int op = open(path,O_CREAT, 0666);
	close(op);
	key_t k = ftok(path, '#');	
	int id = shmget(k, s, 0666 | IPC_CREAT);
	if (id == -1) {
		perror("SHM ERROR");
	}
	int arr[1] = {1};
	semctl(id, 0, SETALL, arr );
	shm_id = id;
	void * p = (void *) shmat(id, NULL,0);
	if ((int)p == -1) {
		perror("OMFG");
	}
	return p;
}

void shm_delete() {
	if (shmctl(shm_id, 0, IPC_RMID, 0) == -1) {
		//perror("ERROR: could not clean up shared memory\n");
	}	
} 