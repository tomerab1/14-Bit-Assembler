#ifndef MEMORY_H
#define MEMORY_H
#include "constants.h"

/*
	This enumeration is used to represent each encoding type with a specific numeric constant. 
*/
typedef enum 
{
	ENCODING_ABS = 0, ENCODING_EXT, ENCODING_RELOC, ENCODING_TOTAL
} EncodingTypes;


/* 
	This enumeration is used to represent each register type with a specific numeric constant. 
*/
typedef enum
{
	REG_R0 = 0, REG_R1, REG_R2, REG_R3,
	REG_R4, REG_R5, REG_R6, REG_R7,
	REG_PSW, REG_PC, REG_TOTAL
} RegistersType;

/*
	This sturct is used to represent a memory word as specified in the project's description.
*/
typedef struct
{
	char e_r_a  : E_R_A_SIZE;
	char source : SOURCE_SIZE;
	char dest   : DEST_SIZE;
	char opcode : OPCODE_SIZE;
	char param1 : PARAM_ONE_SIZE;
	char param2 : PARAM_TWO_SIZE;
} MemoryWord;

/*
	This struct is used to represent 'instruction/data image' and the 'instruction/data counter'.
*/
typedef struct
{
	long counter;
	MemoryWord memory[RAM_MEMORY_SZ];
} imageMemory;

/* 
	This struct is used to represent a 14bit register
*/
typedef struct 
{
	MemoryWord memory;
} Register;

/*
	This struct is used to represent all the memory buffers, the 'cpu' registers and the instruction/data image.
*/
typedef struct
{
	Register registers[REG_TOTAL];
	imageMemory instruction_image;
	imageMemory data_image;
} memoryBuffer;

/*
	This is a function for creating a new memory buffer object.
*/
memoryBuffer memory_buffer_get_new();


/*
	This is an internal function that creates a new image memory object. 
*/
static imageMemory image_memory_get_new();

/*
	This is an internal function used to zero all the image memory. 
*/
static void image_memory_init(imageMemory* mem);


#endif