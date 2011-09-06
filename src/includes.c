/*
 *  includes.c
 *  so-2011
 *
 *  Created by Cristian Pereyra on 05/08/11.
 *  Copyright 2011 My Own. All rights reserved.
 *1
 */

#include <sys/types.h>
#include "includes.h"
#include "data_structures/queue.h"
#include "utils/cstring.h"
#include "utils/sem.h"
#include "utils/colors.h"
#include <stdarg.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int msgq_id = 0;

void clear_msgq() {
	msgctl(msgq_id, IPC_RMID, NULL);	
}

int pointer_comparer(void_p int1, void_p int2) {
	return (int*)int1 - (int*)int2;
}

int int_comparer(void_p int1, void_p int2) {

	if ((int)int2 == 0)
		return 1;

	if ((int)int1 == 0)
		return -1;
	return *(int*)int1 - *(int*)int2;
}

int double_comparer(void_p double1, void_p double2) {
	if (*((double*)double1) < *((double*)double2)) {
		return -1;
	}
	else if (*((double*)double1) > *((double*)double2)) {
		return 1;
	}
	else {
		return 0;
	}
}

int cstring_ds_comparer(void_p s1, void_p s2) {
	return strcmp((char*)s1, *(char**)s2);
}

int cstring_comparer(void_p s1, void_p s2) {
	return strcmp((char*)s1, (char*)s2);
}


void int_printer(void_p int1) {
	printf("%d",*((int*)int1));
}
void double_printer(void_p double1){
	printf("%g",*((double*)double1));
}

void cstring_printer(void_p s1) {
	printf("%s",(char*)s1);
}

void_p pointer_cloner(void_p p1) {
	return int_cloner(p1); //x86 only!
}

void_p int_cloner(void_p int1) {
	int * nuevo = (int *) malloc(sizeof(int));
	*nuevo = *(int*)int1;
	return nuevo;
}
void_p double_cloner(void_p double1){
	double * nuevo = (double *) malloc(sizeof(double));
	*nuevo = *(int*)double1;
	return nuevo;
}

void_p cstring_cloner(void_p s1) {
	char * _s1 = * (char **) s1;
	int len = strlen(_s1);
	char ** nuevo = (char **)malloc(sizeof(char*) * (len+1));
	*nuevo = strcpy(*nuevo, _s1);
	return nuevo;
}

static void separator() {
	printf("--------------------------------\n");
}

int c_sem = 0;

void cprintf(char * format, int color, ...) {
	if (c_sem == 0){
		c_sem = sem_create_typed("cprintf");
		sem_set_value(c_sem, 1);
	}

	sem_down(c_sem, 1);
	va_list args;
	va_start (args, color);
	
	textcolor(color);
	

	vprintf (format, args);	
	
	textattr(CLEAR);
	
	printf("");
	

	va_end (args);	
	sem_up(c_sem, 1);
}

int IPCS_sem = 0;

void IPCSDebug(char * msg, int type, ...) {
	return; ////////// TIRA BUS ERROR EN OSX
	if(!(type&READ) && !(type&WRITE))
		return;
	if (IPCS_sem == 0){
		IPCS_sem = sem_create_typed("IPCSDebug");
		sem_set_value(IPCS_sem, 1);
	}

	sem_down(IPCS_sem, 1);
	va_list args;
	va_start (args, type);

	vfprintf (stderr,msg, args);

	va_end (args);
	sem_up(IPCS_sem, 1);
}

void free_prints_sem() {
	sem_free_typed(IPCS_sem, "IPCSDebug");
	sem_free_typed(c_sem, "cprintf");
}

void tp1_disclaimer() {
	separator();
	
	printf("SISTEMAS OPERATIVOS - ITBA - 2011\n\n");
	printf("Alumnos\n");
	printf("\tMARSEILLAN, AGUSTIN\n");
	printf("\tPEREYRA, CRISTIAN\n");
	printf("\tVIDELA, MAXIMO\n");
	
	separator();
}

void tp1_usage() {
	printf("You can use this program by passing the following parameters:\n");
	
	printf("\t --method (method_name)\n\t\tWhere the method can be 'fifos', 'message_queues', 'shared_memory' or 'sockets'\n");
	printf("\t --level (file_path)\n\t\tPath to the level file.\n");
	printf("\t --airline (file_path) (file_path) ...\n\t\tReads files until it reaches something different. It must be the last param\n");

	separator();
}


int printed = 0;
void _catch(int sig)
{
	if (!printed) {
		cprintf("\nFRONTEND: Bye bye :)\n", ROJO);
		printed = 1;
	}
	killpg(0, sig);  
	nftw("./tmp", (void_p)  unlink_cb, 64, 0);
	shm_delete();
	clear_msgq();
	exit(0);
}

void _catch_child(int sig)
{
	nftw("./tmp", (void_p) unlink_cb, 64, 0);
	shm_delete();
	clear_msgq();
	exit(0);
}


int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, void_p ftwbuf)
{
	list values;
	if (cstring_matches((cstring)fpath, "sem_typed_")) {
		values = cstring_split_list((cstring)fpath, "sem_typed_");
		int sem = sem_create_typed(list_get(values,1));
		sem_free_typed(sem, list_get(values, 1));
	} else if (cstring_matches((cstring)fpath, "sem_")) {
		values = cstring_split_list((cstring)fpath, "sem_");
		int noerror = 0;
		int sem = cstring_parseInt(list_get(values, 1), &noerror);
		int id = sem_create(sem);
		sem_free(id, 0);
	}
    int rv = unlink(fpath);
    return 0;
}
