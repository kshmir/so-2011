#include "sim_smem_transporter.h"

struct sim_smem_transporter {
	int from_id;
	int to_id;
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
	char	blocks_used;			
	char	listens;				
	char	listens_c;				
} smem_header_block;				
									

#define	SMEM_HEADER_BLOCK_SIZE	(sizeof(smem_header_block))
#define	SMEM_HEADER_BLOCK_COUNT	(0x400)
#define	SMEM_HEADER_SIZE		(SMEM_HEADER_BLOCK_SIZE * SMEM_HEADER_BLOCK_COUNT)
#define SMEM_BLOCK_COUNT		(0x400)
#define SMEM_BLOCK_AVAIL_COUNT	(0x3ff)
#define SMEM_BLOCK_MAX_ALLOC	(0xf)
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

typedef struct smem_space {
	char data[SMEM_SPACE_SIZE];
}	smem_space;

smem_space * smem_init_space() {
	if (space == NULL) {
		space = (smem_space *) calloc(sizeof(smem_space),1);
		smem_header_define_block * def = smem_get_header_define(space);
		def->current_write = 0;
		def->available_blocks = SMEM_BLOCK_AVAIL_COUNT;
	}
}

smem_header_define_block * smem_get_header_define(smem_space * s) {
	return ((smem_header_define_block *) s);
}

smem_header_block * smem_get_header_block(smem_space * s, int index) {
	return &((smem_header_block *) s)[index + 1];
}

smem_block * smem_get_block(smem_space * s, int index) {
	return &((smem_block*) s)[index + 1];
}

smem_header_block * smem_get_next_header_block(smem_space * s, int * block_qty) {
	smem_header_define_block * def = smem_get_header_define(s);

	// Available blocks semaphore down.
	if (*block_qty > def->available_blocks) {
		*block_qty = def->available_blocks;
	}
	
	int index =  def->current_write_index;
	smem_block next = smem_get_block(s,index);
	while (index < SMEM_BLOCK_AVAIL_COUNT && next->listens > 0 && next->listens > next->listens_c) {
		next = smem_get_block(s,index++);
	}
	
	return next;
}

smem_block * smem_get_next_header_block(smem_space * s, int index, int * block_qty) {
	
}

void smem_set_block_burst(smem_space * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags &= 0x8000 * value;
}

int smem_get_block_burst(smem_space * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return (b->flags >> 0xf) & 0x1;
}

void smem_set_block_broadcast(smem_space * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags &= 0x4000 * value;
}

int smem_get_block_broadcast(smem_space * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return (b->flags >> 0xe) & 0x1;
}

void smem_set_block_written(smem_space * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags &= 0x2000 * value;
}

int smem_get_block_written(smem_space * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return (b->flags >> 0xd) & 0x1;
}

void smem_set_block_reading(smem_space * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags &= 0x1000 * value;
}

int smem_get_block_reading(smem_space * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return (b->flags >> 0xc) & 0x1;
}


void smem_set_block_allocd(smem_space * s, int index, int value) {
	smem_block * b = smem_get_block(s,index);
	b->flags &= 0x800 * value;
}

int smem_get_block_allocd(smem_space * s, int index) {
	smem_block * b = smem_get_block(s,index);
	return (b->flags >> 0xb) & 0x1;
}

void smem_set_block_next(smem_space * s, int index, short value) {
	smem_block * b = smem_get_block(s,index);
	b->next = value;
}

short smem_get_block_next(smem_space * s, int index, short value) {
	smem_block * b = smem_get_block(s,index);
	return b->next;
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

void smem_space_write(smem_space * s, cstring data) {

	cstring _data_buffer = data;
	int data_qty = 1 + (cstring_len(data) / SMEM_DATA_SIZE);
	int blocks_written = data_qty;
	smem_header_block * header_b = smem_get_next_header_block(s,&blocks_written); // Toma un slot, o espera.
	smem_header_block * header_cur = header_b;
	smem_block	* block_cur
	// write semaphore down.
	
	header_b->mode = 0;											  // Only works one-to-one now.
	header_b->ref_id = 0; // sender_id								  // Points to to_id
	header_b->block_id = smem_get_next_block(s, 0, &blocks_written);		  // Toma sus slots para escribir.
	header_b->blocks_used = blocks_written;
	header_b->listens = 1;
	header_b->listens_c = 0;
	
	// set block semaphore as blocks_used.
	// block semaphore down.
	int end = 0;
	while(blocks_written > 0 && !end) {
		cstring data = cstring_copy_len(_data_buffer, SMEM_BLOCK_AVAIL_COUNT);
		end = smem_block_write(header_b, data, &end);
		if (!end) {
			short next_id = smem_get_next_block(s, header_cur->block_id, &data_qty);
			header_cur = smem_get_block(s, next_id);
			
		}
		blocks_written--;
	}
	
	
	
	
	
	// write semaphore up.
}

smem_space * space = NULL;

sim_smem_transporter sim_smem_transporter_init() {
	space = smem_init_space(space);
}

sim_smem_transporter sim_smem_transporter_open(int connection_id) {
	
}

void sim_smem_transporter_write(sim_smem_transporter t, cstring data) {
	
}

cstring sim_smem_transporter_listen(sim_smem_transporter t, void_p extra_data) {
	
	
}

void sim_smem_transporter_free(sim_smem_transporter transp) {

}