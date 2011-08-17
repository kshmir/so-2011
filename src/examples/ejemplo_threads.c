/*
 *  ejemplo_threads.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 02/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_THREADS     400

void_p TaskCode(void_p argument)
{
	int tid;
	
	tid = *((int *) argument);
	
	printf("Hello World! It's me, thread %d!\n", tid);
	
	/* optionally: insert more useful stuff here */
	
	return NULL;
}

int main (int argc, char *argv[])
{
	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];
	int rc, i;
	
	/* create all threads */
	for (i=0; i<NUM_THREADS; ++i) {
		thread_args[i] = i;
		printf("In main: creating thread %d\n", i);
		rc = pthread_create(&threads[i], NULL, TaskCode, (void_p ) &thread_args[i]);
		assert(0 == rc);
	}
	
	
	/* wait for all threads to complete */
	for (i=0; i<NUM_THREADS; ++i) {
		rc = pthread_join(threads[i], NULL);
		assert(0 == rc);
	}
	
	exit(EXIT_SUCCESS);
}