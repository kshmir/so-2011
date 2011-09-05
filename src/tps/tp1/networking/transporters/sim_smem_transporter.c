/*
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 * Shared Memory Implementation.
 * Don't panic if you don't understand. You'll get through it after reading this and the report.
 * Basically it works in the following way: 
 *   1. The transporter starts up building:
 *      a. One single global shared memory pool.
 *      b. An alloc semaphore (which acts as a mutex). Only one process at a time can alloc space.
 *      c. A blocks available semaphore. If we run out of blocks, then the writing blocks until it gets space.
 *      d. A read available semaphore, so that the read method gets blocked until it really has something to read.
 *      e. There might be other semaphores...
 *   2. The transporter declares a define header.
 *      Has counters for optimizing performance.
 *      
 *   3. This is a basic memory map (don't use line wrapping!)
 *    
 *   _0________________________1________________________2_____________________N___________________
 *  0|smem_header_define_block|smem_block #0:          |smem_block #1:       |smem_block #N - 1:
 *   |------------------------|flags and next_id on    |                     |
 *  1|smem_header_block #0    |first byte. The rest    |                     |
 *   |------------------------|is data data data data d|                     |
 *  2|smem_header_block #1    | data data data data dat|                     |                        ...
 *   |------------------------| data data data data dat|                     |
 *  3|smem_header_block #2    | data data data data dat|                     |
 *   |------------------------| ...                    |                     |
 *  4|smem_header_block #3    |                        |                     |                        ...
 *   |------------------------|                        |                     |
 *  5|smem_header_block #4    |                        |                     | 
 *   |------------------------|                        |                     |
 *  6|smem_header_block #5    |                        |                     |                        ...
 *   |------------------------|                        |                     |
 *  N|smem_header_block #N - 1|                        |                     |
 *   |__________...___________|________________________|_____________________|_____________________
 *
 *   4. When writing:
 *		a. A header is asked via get_next_header_block. It might block if there are no blocks available.
 *         There are always header blocks if there are blocks since blocks_available <= header_blocks_available.
 *         This is because one header might ask for more than 1 block. 
 *         This limit is defined in the SMEM_BLOCK_MAX_ALLOC constant.      
 *      b. This header is filled in the following way:
 *         - Sets the mode. (Currently only one to one, we left the chance open to have broadcasting).
 *         - Sets the ref_id. Which is to whom this message will be sent.
 *         - Gets a block and puts sets it's id as block_id.
 *         - Sets the number of blocks which will be sent. This might be deprecated since
 *           it limits us to a maximum of 255 x SMEM_BLOCK_SIZE data, and because we don't even use it.
 *         - Sets the number of listeners.
 *         - Sets the number of listeners_c where c stands for count, when listeners = listeners_c, then we're done reading
 *           And it can be used again. 
 *      c. After doing this, each needed block is asked via get_next_block and filled with data until it ends with 0. 
 *         After writing, the reader semaphore is risen.
 *   5. When reading:
 *      a. An unblock is waited on the reader semaphore.
 *      b. A matching header is received. (matching == has the ID of the reader).
 *      c. Each block is iterated until it reaches a 0 character value.
 *      d. The alloc values are set to 0, freeing up the blocks.
 *      e. The data is returned.
 */

#include "sim_smem_transporter.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>



/**
	smem_space is the declaration of the shared memory space we alloc.
 */
typedef struct smem_space smem_space;


/**
	Handles all the data required for communicating vía shared memory.
 */
struct sim_smem_transporter {
	int				from_id;				// Id which is used to read.
	int				to_id;					// Id which is used to write.
	int				read_index;				// Last read index.
	int				is_server;				// Helps to change the server logic.
	
	smem_space *	space;					// Pointer to the shared memory.
	
	int				sem_block;				// Semaphore for blocks written.	Used for streaming, not yet done.
	 
	int				sem_block_r;			// Semaphore for blocks to read.	Used for streaming, not yet done.
	
	int				sem_header_w;			// Semaphore for sending up the to_id's value. So it can read when needed.
	
	int				sem_header_r;			// Semaphore for checking if the transporter can read.
	
	int				sem_available_blocks;	// Semaphore for setting the amount of available blocks.
	
	int				sem_alloc;				// Mutex-Like semaphore for blocking the allocation of resources.
};

/**
	Initial header block
 */
typedef struct smem_header_define_block {
	short current_header_index;				// Pointer to the current header for writing.
	short current_block_index;				// Pointer to the current block for writing.	
	short available_blocks;					// Amount of available blocks
	short total_available_blocks;			// Totally redundant, for future use.
} smem_header_define_block;			

/**
	Header block, there are many of these in a bigger block.
 */
typedef struct smem_header_block {	
	char	mode;							// Sets the kind of writing mode, 0 for point to point, 1 for broadcast.
	/** 
		Sets the referral id, it's to whom this block belongs. 
			If it's the p2p mode, then it's the reader.
			If it's broadcast mode, then it's the writer.
	 */
	short   ref_id;							
	/**
		Id of the initial writing block.
	 */
	short	block_id;				
	char	blocks_to_send;					// Number of blocks to send in the message, might be deprecated.
	char	listens;						// Number of listens it should reach till end. Just one for p2p.
	char	listens_c;						// Number of listens done.
} smem_header_block;				



#ifdef __MACH__
/**
 Size fo a header block, it's usually just an int.
 */
#define	SMEM_HEADER_BLOCK_SIZE	(sizeof(smem_header_block))
/**
 Amount of headers.
 */
#define	SMEM_HEADER_BLOCK_COUNT	(0x400 / 5)
/**
 Actual size of the first block.
 */
#define	SMEM_HEADER_SIZE		(SMEM_HEADER_BLOCK_SIZE * SMEM_HEADER_BLOCK_COUNT)
/** 
 Amount of blocks in the shmem.
 */
#define SMEM_BLOCK_COUNT		(0x400 / 5)
/**
 Amount of available blocks (blocks - header block).
 */
#define SMEM_BLOCK_AVAIL_COUNT	((0x400 / 5) - 1)
/**
 Limit of allocs allowed per write, if more needed, then the write is done with buffering.
 */
#define SMEM_BLOCK_MAX_ALLOC	(0x400 / 6)
/**
 Size of a mem block
 */
#define	SMEM_BLOCK_SIZE			(SMEM_HEADER_SIZE)
/**
 Size of the contained data in a block. (SMEM_BLOCK_SIZE - flags - next_id)
 */
#define	SMEM_DATA_SIZE			(SMEM_BLOCK_SIZE - sizeof(short) * 2)
/**
 Total shared memory space required.
 */
#define	SMEM_SPACE_SIZE			(SMEM_BLOCK_COUNT * SMEM_BLOCK_SIZE)

#else
/**
	Size fo a header block, it's usually just an int.
 */
#define	SMEM_HEADER_BLOCK_SIZE	(sizeof(smem_header_block))
/**
	Amount of headers.
 */
#define	SMEM_HEADER_BLOCK_COUNT	(0x400)
/**
	Actual size of the first block.
 */
#define	SMEM_HEADER_SIZE		(SMEM_HEADER_BLOCK_SIZE * SMEM_HEADER_BLOCK_COUNT)
/** 
	Amount of blocks in the shmem.
 */
#define SMEM_BLOCK_COUNT		(0x400)
/**
	Amount of available blocks (blocks - header block).
 */
#define SMEM_BLOCK_AVAIL_COUNT	((0x400) - 1)
/**
	Limit of allocs allowed per write, if more needed, then the write is done with buffering.
 */
#define SMEM_BLOCK_MAX_ALLOC	(0x400)
/**
	Size of a mem block
 */
#define	SMEM_BLOCK_SIZE			(SMEM_HEADER_SIZE)
/**
	Size of the contained data in a block. (SMEM_BLOCK_SIZE - flags - next_id)
 */
#define	SMEM_DATA_SIZE			(SMEM_BLOCK_SIZE - sizeof(short) * 2)
/**
	Total shared memory space required.
 */
#define	SMEM_SPACE_SIZE			(SMEM_BLOCK_COUNT * SMEM_BLOCK_SIZE)
#endif


/**
	Block, it's bigger than what's called a header block, it is pointer by them.
 */
typedef struct smem_block {
	/**
		Flags for the shared memory block.
		They act as following.
			0: Burst bit, not actually used, but it intends to write a lot in a big block.
			1: Broadcast bit, not used also, but left just to be done some day.
			2: Written bit, used to say the block has been written, it's actually used!.
			3: Reading bit, used for buffering IOs.
			4. Allocd bit, used to make the block available for communications.
			The other ones are reserver for future use.
	 */ 
	short flags; 
	/*
		Pointer to the next block.
	 */
	short next;
	
	char data[SMEM_DATA_SIZE]; // Actual data of the block. All this big stuff for just an array of chars... wow!
} smem_block;


/**
	Header, it's a block, containing many header blocks.
 */
typedef struct smem_header {
	smem_header_define_block define_block;								// First block of header
	smem_header_block		 blocks[SMEM_HEADER_BLOCK_COUNT - 1];		// Headers.
} smem_header;


/**
	Space contains a lot of blocks.
 */
struct smem_space {
	char data[SMEM_SPACE_SIZE];	// All the data is contained in this little box.
};


/**
	Gets the define of a shared memory transporter
 */
smem_header_define_block * smem_get_header_define(sim_smem_transporter * s) {
	return ((smem_header_define_block *)s->space);
}

/**
	Starts the space of a shared memory transporter, with all it's corresponding semaphores.
 */
void smem_init_space(sim_smem_transporter * s) {
	
	
	s->sem_alloc = sem_create(254);
	s->sem_header_r  = sem_create(s->from_id);	
	s->sem_header_w  = sem_create(s->to_id);	
	s->sem_available_blocks = sem_create(255);




	
	
	s->space = (smem_space *)shm_create(sizeof(smem_space));
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
		
		
		// You don't actually want this little ones to be different than this.
		sem_up(s->sem_available_blocks, def->available_blocks + 1);
		sem_set_value(s->sem_header_r,0);	

		sem_up(s->sem_alloc, 1);
	}


	//IPCSDebug(SMEM_DEBUG,"Starting shared memory reference: from: %d, to: %d\
//		   \nalloc: %d\t%d\nread: %d\t%d\nwrite: %d\t%d\navailable_blocks: %d\t%d\n",
//		   s->from_id, s->to_id,
//		   s->sem_alloc, sem_value(s->sem_alloc),
//		   s->sem_header_r, sem_value(s->sem_header_r),
//		   s->sem_header_w, sem_value(s->sem_header_w),
//		   s->sem_available_blocks, sem_value(s->sem_available_blocks));
	
}

/**
	Gets a header block from a shared memory transporter
 */
smem_header_block * smem_get_header_block(sim_smem_transporter * s, int index) {
	return &((smem_header_block *)s->space)[index + 1];
}

/**
	Gets a block from a shared memory transporter
 */
smem_block * smem_get_block(sim_smem_transporter * s, int index) {
	return &((smem_block*)s->space)[index + 1];
}

int smem_get_block_id(sim_smem_transporter * s, smem_block * block) {
	return (((int)block - (int)s->space) / sizeof(smem_block)) - 1;
}


/**
	Gets a header block.
 */
smem_header_block * smem_get_next_header_block(sim_smem_transporter * s, int * block_qty) {
	smem_header_define_block * def = smem_get_header_define(s);
	
	
	if (*block_qty > SMEM_BLOCK_MAX_ALLOC) {
		*block_qty = SMEM_BLOCK_MAX_ALLOC;
	}

	if (*block_qty >= def->available_blocks) {
		*block_qty = def->available_blocks;
		sem_down(s->sem_available_blocks, def->available_blocks);
	
		def->available_blocks = 0;
		def->total_available_blocks = 0;
	}
	else {
		sem_down(s->sem_available_blocks, *block_qty);
		def->available_blocks -= *block_qty;
		def->total_available_blocks -= *block_qty;
		
	}
	
	int index = def->current_header_index;
	smem_header_block * next = smem_get_header_block(s,index);
	if (index == SMEM_BLOCK_AVAIL_COUNT) {
		index = 0;
	}

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


/*
	Accesors for blocks flags...
*/

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

// Ya fue..........
void smem_set_block_allocd(sim_smem_transporter * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags = value;
}

int smem_get_block_allocd(sim_smem_transporter * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return b->flags;
}

void smem_set_block_next(sim_smem_transporter * s, int index, short value) {
	smem_block * b = smem_get_block(s,index);
	b->next = value;
}

short smem_get_block_next(sim_smem_transporter * s, int index, short value) {
	smem_block * b = smem_get_block(s,index);
	return b->next;
}
/*
	End of accesors for blocks flags.
*/

/**
	Finds the next available block(s) and alloc(s) em.
*/
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
			
			////IPCSDebug(SMEM_DEBUG,"I RESERVE block %d\n",index);
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

/**
	Writes on an allocd block.
 */
int smem_block_write(smem_block * b, char * data) {
	int i = 0;
	for (; i < SMEM_DATA_SIZE; i++) {
		b->data[i] = data[i];
		if (data[i] == 0) {
			return 1;
		}
	}
	return 0;
}

/**
	Accesors for blocks flags...
 */
void smem_space_write(sim_smem_transporter * s, cstring data) {
	
	smem_header_define_block * def = smem_get_header_define(s);
	
	cstring _data_buffer = data;
	int data_qty = 1 + (cstring_len(data) / SMEM_DATA_SIZE);
//	//IPCSDebug(SMEM_DEBUG&WRITE,"DATA_SIZE: %d\n", SMEM_DATA_SIZE);
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
	
	
	////cprintf("DATA TO WRITE: %d %d\n", ROJO, data_qty, SMEM_DATA_SIZE);
	// set block_available semaphore as blocks_written.
	while(!end) {
		// Wait for blocks available.
		// Now we're malloc'd 

		cstring d = cstring_copy_len(_data_buffer, SMEM_DATA_SIZE);
		_data_buffer += SMEM_DATA_SIZE;
//		//IPCSDebug(SMEM_DEBUG&WRITE,"\n\nSTRING LEN: %d, %s\n\n", cstring_len(d), d);
		end = smem_block_write(block_cur, d);
		
		smem_set_block_written(s, block_cur_id, 1);
//		free(d);
		


		int oldVal = sem_value(s->sem_header_w);
		
//		//IPCSDebug(SMEM_DEBUG&WRITE,"UP on write %d\t %d\t old: %d\n END = %d",s->sem_header_w, sem_value(s->sem_header_w), oldVal, end);
		
//		//IPCSDebug(SMEM_DEBUG&WRITE,"I send %s to %d, listens %d block_id %d header_id %d ref_sem %d\n", data,  header_b->ref_id, header_b->listens, block_cur_id, ((int)header_b - (int)s->space) / sizeof(smem_header_block),
//			   sem_value(s->sem_header_w));
		// blocks_available semaphore down.
		// read semaphore up
		
		if (!end) {
			short next_id = block_cur->next;
			block_cur_id = next_id;
			block_cur = smem_get_block(s, next_id);
//			//IPCSDebug(SMEM_DEBUG&WRITE,"%d vs %d", next_id, block_cur_id);
		}

		free(d);
		data_qty--;
		//cprintf("DATA TO WRITE: %d\n", ROJO, data_qty);
	}
	
	sem_up(s->sem_header_w, blocks_to_write);
	cprintf("SMEM: BOUT TO WRITE %d\t %d\t %d\t %d\n",CELESTE, blocks_to_write, sem_value(s->sem_available_blocks), sem_value(s->sem_header_w), sem_value(s->sem_header_r));	
	if (end && data_qty != 0) {
		// //IPCSDebug(SMEM_DEBUG&WRITE,"Amount calculation error!");
	}
	
	
	

}

/**
	Read from shared memory
*/
cstring smem_space_read(sim_smem_transporter * s) {
	
	smem_header_define_block * def = smem_get_header_define(s);
	
	
	// TODO: Fix this!!!!!!!!
//	//IPCSDebug(SMEM_DEBUG&READ,"Read sem value: %d\t %d\n", s->sem_header_r, sem_value(s->sem_header_r));
	cprintf("SMEM_READ: BOUT TO READ: %d\n", AMARILLO, sem_value(s->sem_header_r));
	sem_down(s->sem_header_r, 1);
//	//IPCSDebug(SMEM_DEBUG&READ,"UNLOCK! %d\t %d\n", s->sem_header_r, sem_value(s->sem_header_r));

	


	int read_index = s->read_index;

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
		
		if (read_index == SMEM_BLOCK_COUNT) {
			read_index = 0;
		}
	}
	

	
	smem_block * current_block = smem_get_block(s, current->block_id);
	
	int end = 0;
	int blocks_read = 1;
	cstring response = cstring_init(0);
	while (!end) {
		int block_len = cstring_capped_len(current_block->data, SMEM_DATA_SIZE);
		end = block_len != -1;
		cstring block = cstring_init(0); 
		block = (cstring) cstring_copy_len_in(current_block->data, block, (block_len == -1) ? ((int)SMEM_DATA_SIZE) : block_len);
		response = cstring_write(response, block);
		//cprintf("BLOCKS READ: %d %d\n", ROJO, blocks_read, block_len);
		if (!end) {			
			current_block = smem_get_block(s, current_block->next);
			blocks_read++;
			sem_down(s->sem_header_r, 1);
		}			
		
		free(block);
	}

	def->available_blocks += blocks_read;
	def->total_available_blocks += blocks_read;
	current->listens_c++;
	
	sem_up(s->sem_available_blocks,blocks_read);
	
//	//IPCSDebug(SMEM_DEBUG&READ,"I found listens:%d\tindex:%d\tref_id:%d\tIm:%d\tmsg:%s\t%d\tsem_reader:%d\n",current->listens,read_index,current->ref_id,s->from_id,response,s->is_server,
//		   sem_value(s->sem_header_r) + 1);
	if (current->listens_c == current->listens) {
		//sem_down(s->sem_alloc, 1);
		current->listens = current->listens_c = 0;
		int block_id = current->block_id;
		int c_block_id = -1;
		current_block = smem_get_block(s, current->block_id);
		while (c_block_id != current->block_id) {
			if (c_block_id == -1) {
				c_block_id = block_id;
			}
			c_block_id = smem_get_block_id(s, current_block);
			smem_set_block_allocd(s, c_block_id, 0);
			current_block = smem_get_block(s, current_block->next);
		}

		
		// We should tell the world we dissalloc all the data, shouldn't we??!?!?
//		sem_up(s->sem_alloc, 1);
		
		
		
	}
	s->read_index = read_index;
	return response;
}

/**
	Start a shared memory endpoint.
*/
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

/**
	Write to the shared memoery.
*/
void sim_smem_transporter_write(sim_smem_transporter * t, cstring data) {
	cprintf("SMEM: I want to write %d\n",VERDE, cstring_len(data));

	smem_space_write(t, data);

}

/**
	Listen to the shared memory.
*/
cstring sim_smem_transporter_listen(sim_smem_transporter * t, int * extra_data) {
	cstring data = smem_space_read(t);
	*extra_data = strlen(data) + 1;
	cprintf("SMEM_READ: len: %d\n", ROJO, *extra_data);
	return data;
}


/**
	Free up everything.
 */
void sim_smem_transporter_free(sim_smem_transporter * transp) {
//	sem_free(transp->sem_header_w, transp->to_id);
//	sem_free(transp->sem_header_r, transp->from_id);
//	sem_free(transp->sem_available_blocks, 255);
//	sem_free(transp->sem_alloc, 254);

	shm_delete();
	free(transp);
}

