#include "sim_smem_transporter.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>

typedef struct smem_space smem_space;

static int shm_id;

void * shm_create(int s) {
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
		perror("Semaphore fail on UP");
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








struct sim_smem_transporter {
	int				from_id;
	int				to_id;
	int				read_index;
	int				is_server;
	
	smem_space *	space;
	
	int				sem_header_w;
	
	int				sem_header_r;
	
	int				sem_block;
	
	int				sem_block_r;
	
	int				sem_available_blocks;
	
	int				sem_alloc;
	
	// Semaforo de headers disponibles.			   
	// Semaforo de headers de lectura disponibles. (id-dependant) (reader_id)
	// Semaforo de bloques disponibles.			   (id-dependant) (bloque_id)
	// Semaforo de bloques de lectura disponibles. (id-dependant) (bloque_id)
	
	// Semaforo de alloc.
	
};

typedef struct smem_header_define_block {
	short current_header_index;				
	short current_block_index;					
	short available_blocks;
	short total_available_blocks;
} smem_header_define_block;			

typedef struct smem_header_block {	
	char	mode;					
	short   ref_id;					
	short	block_id;				
	char	blocks_to_send;			
	char	listens;				
	char	listens_c;				
} smem_header_block;				


#define	SMEM_HEADER_BLOCK_SIZE	(sizeof(smem_header_block))
#define	SMEM_HEADER_BLOCK_COUNT	(0x400)
#define	SMEM_HEADER_SIZE		(SMEM_HEADER_BLOCK_SIZE * SMEM_HEADER_BLOCK_COUNT)
#define SMEM_BLOCK_COUNT		(0x400 / 5)
#define SMEM_BLOCK_AVAIL_COUNT	((0x400 / 5) - 1)
#define SMEM_BLOCK_MAX_ALLOC	(0x1)
#define	SMEM_BLOCK_SIZE			(SMEM_HEADER_SIZE)
#define	SMEM_DATA_SIZE			(SMEM_BLOCK_SIZE - sizeof(short) * 2)
#define	SMEM_SPACE_SIZE			(SMEM_BLOCK_COUNT * SMEM_BLOCK_SIZE)

typedef struct smem_block {
	short flags;
	short next;
	
	char data[SMEM_BLOCK_SIZE - sizeof(short) * 2];
} smem_block;

typedef struct smem_header {
	smem_header_define_block define_block;
	smem_header_block		 blocks[SMEM_HEADER_BLOCK_COUNT - 1];
} smem_header;

struct smem_space {
	char data[SMEM_SPACE_SIZE];
};


smem_header_define_block * smem_get_header_define(sim_smem_transporter * s) {
	return ((smem_header_define_block *)s->space);
}

void smem_init_space(sim_smem_transporter * s) {
	
	s->sem_alloc = sem_create(254);
	s->sem_header_r  = sem_create(s->from_id);	
	s->sem_header_w  = sem_create(s->to_id);	
	s->sem_available_blocks = sem_create(255);
	

//	printf("I build %d\t %d\t %d\t %d\n", s->sem_header_w, s->sem_header_r, s->sem_alloc, s->sem_available_blocks);
	
	
	s->space = shm_create(sizeof(smem_space));
/*	int i = 0;
	for (i = 0; i < sizeof(smem_space); i++) {
		s->space->data[i] = 0;
	}*/
	smem_header_define_block * def = smem_get_header_define(s);
	if (def->available_blocks == 0) {
		def->current_header_index = 0;
		def->current_block_index = 0;
		def->available_blocks = SMEM_BLOCK_AVAIL_COUNT;
		def->total_available_blocks = SMEM_BLOCK_AVAIL_COUNT;
		sem_up(s->sem_available_blocks, SMEM_BLOCK_AVAIL_COUNT + 1);
		sem_up(s->sem_alloc, 1);
	}

	
	
}




smem_header_block * smem_get_header_block(sim_smem_transporter * s, int index) {
	return &((smem_header_block *)s->space)[index + 1];
}

smem_block * smem_get_block(sim_smem_transporter * s, int index) {
	return &((smem_block*)s->space)[index + 1];
}

smem_header_block * smem_get_next_header_block(sim_smem_transporter * s, int * block_qty) {
	smem_header_define_block * def = smem_get_header_define(s);
	
	
	if (*block_qty > SMEM_BLOCK_MAX_ALLOC) {
		*block_qty = SMEM_BLOCK_MAX_ALLOC;
	}
	
	if (*block_qty >= def->available_blocks) {
		*block_qty = def->available_blocks;
		
		sem_down(s->sem_available_blocks, def->available_blocks);
	
		def->available_blocks = 0;
		sem_down(s->sem_available_blocks, 1);
	}
	else {
		sem_down(s->sem_available_blocks, *block_qty);
		def->available_blocks -= *block_qty;
		
	}
	
	int index = def->current_header_index;
	smem_header_block * next = smem_get_header_block(s,index);
	while (next->listens > 0 && next->listens > next->listens_c) {
		next = smem_get_header_block(s,index + 1);
		index++;
		if (index == SMEM_BLOCK_AVAIL_COUNT) {
			index = 0;
		}
	}
	def->current_header_index = index + 1;
	
	if (def->current_header_index == SMEM_BLOCK_AVAIL_COUNT) {
		def->current_header_index = 0;
	}
	
	return next;
}

void smem_set_block_burst(sim_smem_transporter * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags |= 0x8000 * value;
}

int smem_get_block_burst(sim_smem_transporter * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return (b->flags >> 0xf) & 0x1;
}

void smem_set_block_broadcast(sim_smem_transporter * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags |= 0x4000 * value;
}

int smem_get_block_broadcast(sim_smem_transporter * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return (b->flags >> 0xe) & 0x1;
}

void smem_set_block_written(sim_smem_transporter * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags |= 0x2000 * value;
}

int smem_get_block_written(sim_smem_transporter * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return (b->flags >> 0xd) & 0x1;
}

void smem_set_block_reading(sim_smem_transporter * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags |= 0x1000 * value;
}

int smem_get_block_reading(sim_smem_transporter * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return (b->flags >> 0xc) & 0x1;
}


void smem_set_block_allocd(sim_smem_transporter * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags |= 0x800 * value;
}

int smem_get_block_allocd(sim_smem_transporter * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return (b->flags >> 0xb) & 0x1;
}

void smem_set_block_next(sim_smem_transporter * s, int index, short value) {
	smem_block * b = smem_get_block(s,index);
	b->next = value;
}

short smem_get_block_next(sim_smem_transporter * s, int index, short value) {
	smem_block * b = smem_get_block(s,index);
	return b->next;
}


short smem_get_next_block_and_alloc(sim_smem_transporter * s, int * block_qty) {
	smem_header_define_block * def = smem_get_header_define(s);
	int qty = * block_qty;
	int index = def->current_block_index;
	char start_id_set = 0;
	short start_id = 0;
	short last_id = 0;
	while(qty > 0) {
		if (smem_get_block_allocd(s, index) == 0) {
			smem_set_block_allocd(s, index, 1);
			smem_set_block_broadcast(s, index, 0);
			smem_set_block_burst(s, index, 0);
			smem_set_block_written(s, index, 0);
			smem_set_block_reading(s, index, 0);
			if (!start_id_set) {
				start_id_set = 1;
				start_id = index;
			} else {
				smem_set_block_next(s, last_id, index);
			}
			last_id = index;
			
			qty--;			
		}
		
		index++;
		
		if (index == SMEM_BLOCK_AVAIL_COUNT) {
			index = 0;
		}
	}
	
	def->current_block_index = index;
	smem_set_block_next(s, (int)last_id, (int)start_id); // Cierra el buffer circular.
	
	return start_id;
}


int smem_block_write(smem_block * b, char * data) {
	int i = 0;
	for (; i < SMEM_BLOCK_AVAIL_COUNT; i++) {
		b->data[i] = data[i];
		if (data[i] == 0) {
			return 1;
		}
	}
	return 0;
}

void smem_space_write(sim_smem_transporter * s, cstring data) {
	
	smem_header_define_block * def = smem_get_header_define(s);
	cstring _data_buffer = data;
	int data_qty = 1 + (cstring_len(data) / SMEM_DATA_SIZE);
	int blocks_to_write = data_qty;
	smem_header_block * header_b = smem_get_next_header_block(s,&blocks_to_write); // Toma un slot, o espera.
	smem_header_block * header_cur = header_b;
	smem_block	* block_cur = NULL;
	
	sem_down(s->sem_alloc, 1);
	header_b->mode = 0;																// Only works one-to-one now.
	header_b->ref_id = s->to_id;
	header_b->block_id = smem_get_next_block_and_alloc(s, &blocks_to_write);		// Toma sus slots para escribir.
	header_b->blocks_to_send = data_qty;
	header_b->listens = 1;
	header_b->listens_c = 0;
	sem_up(s->sem_alloc, 1);
	
	
	
	short block_cur_id = header_b->block_id;
	int end = 0;
	block_cur = smem_get_block(s, block_cur_id);
	
	
	
	// set block_available semaphore as blocks_written.
	while(!end) {
		// Wait for blocks available.
		// Now we're malloc'd 
		cstring d = cstring_copy_len(_data_buffer, SMEM_BLOCK_AVAIL_COUNT);
		_data_buffer += SMEM_BLOCK_AVAIL_COUNT;
		end = smem_block_write(block_cur, d);
		smem_set_block_written(s, block_cur_id, 1);
//		free(d);
		


		
		sem_up(s->sem_header_w, 1);
//		printf("I send %s to %d, listens %d block_id %d header_id %d ref_sem %d\n", data,  header_b->ref_id, header_b->listens, block_cur_id, ((int)header_b - (int)s->space) / sizeof(smem_header_block),
//			   sem_value(s->sem_header_w));
		// blocks_available semaphore down.
		// read semaphore up
		
		short next_id = block_cur->next;
		block_cur_id = next_id;
		block_cur = smem_get_block(s, next_id);
		data_qty--;
		
	}
	
	if (end && data_qty != 0) {
		printf("Amount calculation error!");
	}
	
	
	
	
}


cstring smem_space_read(sim_smem_transporter * s) {
	
	smem_header_define_block * def = smem_get_header_define(s);
	
	
	// TODO: Fix this!!!!!!!!
	//sem_down(s->sem_header_r, 1);
	

	int read_index = (s->read_index == 0) ? s->read_index : (s->read_index - 1);
	int found = 0;
	smem_header_block * current = NULL;
	while (!found) {
		current = smem_get_header_block(s, read_index);
		
		
		// Update this.
		if (current->ref_id == s->from_id && current->listens > 0 && current->listens > current->listens_c && current->mode == 0) {
			found = 1;
		} else {
			read_index++;
		}	
		
		if (read_index == SMEM_BLOCK_COUNT - 1) {
			read_index = 0;
		}
	}

	
	smem_block * current_block = smem_get_block(s, current->block_id);
	
	int end = 0;
	int blocks_read = 1;
	cstring response = cstring_init(0);
	while (!end) {
		int block_len = cstring_capped_len(current_block->data, SMEM_BLOCK_AVAIL_COUNT);
		end = block_len != -1;
		response = (cstring) cstring_copy_len_in(current_block->data, response, (block_len == -1) ? ((int)SMEM_BLOCK_AVAIL_COUNT) : block_len);
		if (!end) {			
			current_block = smem_get_block(s, current_block->next);
			blocks_read++;
		}
	}
	def->available_blocks += blocks_read;
	current->listens_c++;
	
	sem_up(s->sem_available_blocks,blocks_read);
	
//	printf("I found listens:%d\tindex:%d\tref_id:%d\tIm:%d\tmsg:%s\t%d\tsem_reader:%d\n",current->listens,read_index,current->ref_id,s->from_id,response,s->is_server,
//		   sem_value(s->sem_header_r) + 1);
	if (current->listens_c == current->listens) {
		current->listens = current->listens_c = 0;
	}
	s->read_index = read_index;
	return response;
}

sim_smem_transporter * sim_smem_transporter_init(int server_id, int client_id, int is_server) {
	sim_smem_transporter * s = (sim_smem_transporter *) calloc(sizeof(sim_smem_transporter),1);
	if (is_server) {
		s->from_id = server_id;
		s->to_id = client_id;
		
	}
	else {
		
		s->to_id = server_id;
		s->from_id = client_id;	
	}
	
	s->is_server = is_server;
	smem_init_space(s);
	s->read_index = 0;
	return s;
}

void sim_smem_transporter_write(sim_smem_transporter * t, cstring data) {
	smem_space_write(t, data);
}

cstring sim_smem_transporter_listen(sim_smem_transporter * t, void_p extra_data) {
	cstring data = smem_space_read(t);
	*(int*)extra_data = strlen(data) + 1;
	return data;
}

void sim_smem_transporter_free(sim_smem_transporter * transp) {

	sem_free(transp->sem_header_w);
	sem_free(transp->sem_header_r);
	sem_free(transp->sem_available_blocks);
	sem_free(transp->sem_alloc);
	shm_delete();
	free(transp);
}

