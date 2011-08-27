/*
 *  shm.h
 *  so-2011
 *
 *  Created by Cristian Pereyra on 27/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>


#ifndef _SEM_H_
#define _SEM_H_
/**
 Creates a shared memory of the size given.
 Returns -1 if it's invalid.
 Returns NULL if already allocd.
 Returns something else if it works.
 */
void * shm_create(size_t s);

/**
 Deletes the already allocated shared memory.
 */
void shm_delete();
#endif