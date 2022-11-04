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
	char e_r_a  : 2;
	char source : 2;
	char dest   : 2;
	char opcode : 4;
	char param1 : 2;
	char param2 : 2;
} MemoryWord;

/*
	This struct is used to represent the 'random access memory' (RAM).
*/
typedef struct
{
	MemoryWord memory[RAM_MEMORY_SZ];
} RAM;

/* 
	This struct is used to represent a 14bit register
*/
typedef struct 
{
	MemoryWord memory;
} Register;

/*
	This struct is used to represent a CPU and it's internal registers.
*/
typedef struct
{
	unsigned int DC;
	Register registers[REG_TOTAL];
	RAM ram;
} CPU;

/*
	This is a function for creating a new CPU object.
*/
CPU get_new_cpu();


/*
	This is an internal function that creates a new RAM object. 
*/
static RAM get_new_ram();

/*
	This is an internal function used to zero all the ram memory. 
*/
static void init_ram(RAM* ram);


#endif