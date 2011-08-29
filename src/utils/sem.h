/*
 *  sem.h
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

// Starts a semaphore.
int sem_create(int key);

// Gets the value of a semaphore.
int sem_value(int sem);

// Sends a semaphore up.
int sem_up(int sem, int amount);

// Sends a semaphore down.
int sem_down(int sem, int amount);

// Free's a semaphore.
int sem_free(int sem);

#endif