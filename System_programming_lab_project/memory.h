#ifndef MEMORY_H
#define MEMORY_H

/** @file
*	This header is used to specify all memory related data structures, i.e. MemoryWord, memoryBuffer etc.
*   This header also includes all the appropriate routines for manipulating and creating those data structures.
*/

#include "constants.h"

#define SIZEOF_MEMORY_WORD 2 /* We only need 14 bits, so we will use an array of 2 chars. */

#define FLAG_ERA     0x01
#define FLAG_SOURCE  0x02
#define FLAG_DEST    0x04
#define FLAG_OPCODE1 0x08
#define FLAG_OPCODE2 0x10
#define FLAG_PARAM1  0x20
#define FLAG_PARAM2  0x40

#define MASK_ERA     0x03
#define MASK_DEST    0x0c
#define MASK_SOURCE  0x30
#define MASK_OPCODE1 0xc0
#define MASK_OPCODE2 0x03
#define MASK_PARAM1  0x0c
#define MASK_PARAM2  0x30

#define OFFSET_ERA     0x00
#define OFFSET_DEST    0x02
#define OFFSET_SOURCE  0x04
#define OFFSET_OPCODE1 0x06
#define OFFSET_OPCODE2 0x00
#define OFFSET_PARAM1  0x02
#define OFFSET_PARAM2  0x04

/**
	This enumeration is used to represent each encoding type with a specific numeric constant. 
*/
typedef enum 
{
	ENCODING_ABS = 0, ENCODING_EXT, ENCODING_RELOC
} EncodingTypes;


/** 
	This enumeration is used to represent each register type with a specific numeric constant. 
*/
typedef enum
{
	REG_R0 = 0, REG_R1, REG_R2, REG_R3,
	REG_R4, REG_R5, REG_R6, REG_R7,
	REG_PSW, REG_PC, REG_TOTAL
} RegistersType;

/**
	@brief This sturct is used to represent a memory word as specified in the project's description.
*/
typedef struct
{
	unsigned char mem[SIZEOF_MEMORY_WORD];
	int encodingCount;
} MemoryWord;

/**
	@brief This struct is used to represent 'instruction/data image' and the 'instruction/data counter'.
*/
typedef struct
{
	long counter;
	MemoryWord memory[RAM_MEMORY_SZ];
} imageMemory;

typedef struct
{
	MemoryWord memory;
}Register;

/**
	@brief This struct is used to represent all the memory buffers, the 'cpu' registers and the instruction/data image.
*/
typedef struct
{
	imageMemory instruction_image;
	imageMemory data_image;
	Register registers[REG_TOTAL];
} memoryBuffer;

/**
	@brief This is a function for creating a new memory buffer object.
*/
memoryBuffer memory_buffer_get_new();

/** @brief This function sets bytes appropriately in a memoryWord
*/
void set_image_memory(imageMemory* mem, unsigned char bytes, int flags);


/**
	This is an internal function that creates a new image memory object. 
	@return - A new image memory structure.
*/
static imageMemory image_memory_get_new();

/**
	@brief This is an internal function used to zero all the image memory. 
*/
static void image_memory_init(imageMemory* mem);

/** @brief This function is for setting the e.r.a flag */
void set_era_bits(MemoryWord* mem, unsigned char byte);

/** @brief This function is for setting the e.r.a flag */
void set_source_bits(MemoryWord* mem, unsigned char byte);

/** @brief This function is for setting the e.r.a flag */
void set_dest_bits(MemoryWord* mem,  unsigned char byte);

/** @brief This function is for setting the e.r.a flag */
void set_opcode_cell_1_bits(MemoryWord* mem, unsigned char byte);

/** @brief This function is for setting the e.r.a flag */
void set_opcode_cell_2_bits(MemoryWord* mem, unsigned char byte);

/** @brief This function is for setting the e.r.a flag */
void set_param1_bits(MemoryWord* mem, unsigned char byte);

/** @brief This function is for setting the e.r.a flag */
void set_param2_bits(MemoryWord* mem, unsigned char byte);


/* This function is for getting the e.r.a flag */
unsigned char get_era_bits(MemoryWord* mem);

/* This function is for getting the e.r.a flag */
unsigned char get_source_bits(MemoryWord* mem);

/* This function is for getting the e.r.a flag */
unsigned char get_dest_bits(MemoryWord* mem);

/* This function is for getting the e.r.a flag */
unsigned char get_opcode_cell_1_bits(MemoryWord* mem);

/* This function is for getting the e.r.a flag */
unsigned char get_opcode_cell_2_bits(MemoryWord* mem);

/* This function is for getting the e.r.a flag */
unsigned char get_param1_bits(MemoryWord* mem);

/* This function is for getting the e.r.a flag */
unsigned char get_param2_bits(MemoryWord* mem);

void dump_memory(memoryBuffer* buf);

#endif