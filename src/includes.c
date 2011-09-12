/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 *
 * @file includes.c
 *
 * @brief Provides functions shared across all the programs.
 *
 * @author Agustin Marseillan
 * @author Maximo Videla
 * @author Cristian Pereyra
 *
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

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int msgq_id = 0;

static list declared_threads = NULL;

void declare_thread(pthread_t * t) {
	if (declared_threads == NULL) {
		declared_threads = list_init();
	}
	list_add(declared_threads, t);
}


/**
	Cancels all thread references from the program.
 */
static void cancel_all_threads() {
	if (declared_threads != NULL) {
		foreach(pthread_t *, t, declared_threads) {
			if (t != NULL) {
				pthread_detach(* t);
				pthread_cancel(* t);
			}
		}
	}
}
/**
	Used to clear the message queue reference if used.
 */
static void clear_msgq() {
	if (msgq_id != 0) {
		msgctl(msgq_id, IPC_RMID, NULL);	
	}
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

/**
	Used for showing a line...
 */
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


/**
 * Clears interatively all semaphores.
 */
int clean_semaphore(const char *fpath, const struct stat *sb, int typeflag, void_p ftwbuf)
{
	list values;
	if (cstring_matches((cstring)fpath, "sem_typed_")) {
		values = cstring_split_list((cstring)fpath, "sem_typed_");
		int sem = sem_create_typed(list_get(values,1));
		sem_free_typed(sem, list_get(values, 1));
		list_free_with_data(values);
	} else if (cstring_matches((cstring)fpath, "sem_")) {
		values = cstring_split_list((cstring)fpath, "sem_");
		int noerror = 0;
		int sem = cstring_parseInt(list_get(values, 1), &noerror);
		int id = sem_create(sem);
		sem_free(id, 0);
		list_free_with_data(values);
	}
    int rv = unlink(fpath);
    return 0;
}

int printed = 0;
int freed = 0;
void _catch(int sig)
{

	if (!printed) {
		cprintf("\nFRONTEND: Bye bye :)\n", ROJO);
		printed = 1;
	}
	killpg(0, sig);  
	
	if (!freed) {
		freed = 1;

		clean_exit();
	}
	
	exit(0);
}


void segfault_handler(int sig) {
	void *array[10];
	size_t size;
	
	// get void*'s for all entries on the stack
	size = backtrace(array, 10);
	
	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, 2);
	exit(0);
}

void _catch_child(int sig)
{
	killpg(0, sig);  
	
	if (!freed) {
		freed = 1;
		
		clean_exit();
	}
	
	
	exit(0);
}

void clean_exit() {
	cancel_all_threads();
	clear_msgq();
	shm_delete();	
    nftw("./tmp",  (void_p) clean_semaphore, 64, 0);
	
	
	
	free_root();
	

}
