#ifndef MEMORY_H
#define MEMORY_H

/** @file
*	This header is used to specify all memory related data structures, i.e. MemoryWord, memoryBuffer etc.
*   This header also includes all the appropriate routines for manipulating and creating those data structures.
*/

#include "constants.h"

#define SIZEOF_MEMORY_WORD 2 /* We only need 14 bits, so we will use an array of 2 chars. */

/**
* @brief Useful constants for using the 'set' functions.
*/
#define FLAG_ERA     0x01
#define FLAG_SOURCE  0x02
#define FLAG_DEST    0x04
#define FLAG_OPCODE1 0x08
#define FLAG_OPCODE2 0x10
#define FLAG_PARAM1  0x20
#define FLAG_PARAM2  0x40

/**
* @brief This enumeration is used to represent each encoding type with a specific numeric constant. 
*/
typedef enum 
{
	ENCODING_ABS = 0, ENCODING_EXT, ENCODING_RELOC
} EncodingTypes;


/** 
* @brief This enumeration is used to represent each register type with a specific numeric constant. 
*/
typedef enum
{
	REG_R0 = 0, REG_R1, REG_R2, REG_R3,
	REG_R4, REG_R5, REG_R6, REG_R7,
	REG_PSW, REG_PC, REG_TOTAL
} RegistersType;

/**
* @brief This sturct is used to represent a memory word as specified in the project's description.
*/
typedef struct MemoryWord MemoryWord;

/**
* @brief This struct is used to represent 'instruction/data image' and the 'instruction/data counter'.
*/
typedef struct imageMemory imageMemory;


/**
* @brief This struct is used to represent all the memory buffers, the 'cpu' registers and the instruction/data image.
*/
typedef struct memoryBuffer memoryBuffer;

/**
* @brief This is a function for creating a new memory buffer object.
*/
memoryBuffer* memory_buffer_get_new();

/** 
* @brief This function sets bytes appropriately in a memoryWord according to the specified flags.
* @param mem - The imageMemory
* @param bytes - The byte.
* @param flags - The flags.
*/
void set_image_memory(imageMemory* mem, unsigned char byte, int flags);

/**
* @breif This is an internal function that creates a new image memory object. 
* @return A new image memory structure.
*/
imageMemory* image_memory_get_new();

/**
* @brief This is an internal function used to zero all the image memory.
* @param mem - The image memory.
*/
void image_memory_init(imageMemory* mem);

imageMemory* memory_buffer_get_data_img(memoryBuffer* memBuff);

imageMemory* memory_buffer_get_inst_img(memoryBuffer* memBuff);

int img_memory_get_counter(imageMemory* im);

void img_memory_set_counter(imageMemory* im, int cnt);

MemoryWord* img_memory_get_memory_at(imageMemory* im, int offset);

MemoryWord* img_memory_get_memory(imageMemory* im);

unsigned char* memory_word_get_memory(MemoryWord* mw);

/**
*  The memory word structure:

*  Param2 | Param 1 | Opcode | Dest Operand | Source Operand | E.R.A
* ---------------------------------------------------------------------
* | 2 bits | 2 bits | 4 bits |     2 bits   |      2 bits    | 2 bits |
* ---------------------------------------------------------------------
* We divided the Opcode to 2 parts, Op1 and Op2 to align each section on 1 byte:

* Param2 | Param 1  |   Op1  |   Op2  | Dest Operand | Source Operand | E.R.A
* -----------------------------------------------------------------------------
* | 2 bits | 2 bits | 2 bits | 2 bits |     2 bits   |      2 bits    | 2 bits |
* ------------------------------------------------------------------------------
* ---------second byte------- ------------------first byte --------------------

* This way its easier to set the bits using our 'set' functions.
*/

/** 
* @brief This function is for setting the e.r.a flag to the value of byte.
* @param mem - The image memory.
* @param byte - The byte.
*/
void set_era_bits(MemoryWord* mem, unsigned char byte);

/**
* @brief This function is for setting the source flag to the value of byte.
* @param mem - The image memory.
* @param byte - The byte.
*/
void set_source_bits(MemoryWord* mem, unsigned char byte);

/**
* @brief This function is for setting the dest flag to the value of byte.
* @param mem - The image memory.
* @param byte - The byte.
*/
void set_dest_bits(MemoryWord* mem,  unsigned char byte);

/**
* @brief This function is for setting the opcode_1 flag to the value of byte.
* @param mem - The image memory.
* @param byte - The byte.
*/
void set_opcode_cell_1_bits(MemoryWord* mem, unsigned char byte);

/**
* @brief This function is for setting the opcode_2 flag to the value of byte.
* @param mem - The image memory.
* @param byte - The byte.
*/
void set_opcode_cell_2_bits(MemoryWord* mem, unsigned char byte);

/**
* @brief This function is for setting the param_1 flag to the value of byte.
* @param mem - The image memory.
* @param byte - The byte.
*/
void set_param1_bits(MemoryWord* mem, unsigned char byte);

/**
* @brief This function is for setting the param_2 flag to the value of byte.
* @param mem - The image memory.
* @param byte - The byte.
*/
void set_param2_bits(MemoryWord* mem, unsigned char byte);

#endif
