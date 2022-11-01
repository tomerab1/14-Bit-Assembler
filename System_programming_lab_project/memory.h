#ifndef MEMORY_H
#define MEMORY_H

/*
	This enumeration is used to represent each encoding type with a specific numeric constant. 
*/
typedef enum 
{
	ENCODING_ABS = 0, ENCODING_EXT, ENCODING_RELOC, ENCODING_TOTAL
} EncodingTypes;


/*
	This sturct is used to represent a memory word as specified in the project's description.
*/
typedef struct 
{
	char e_r_a : 2;
	char source : 2;
	char dest : 2;
	char opcode : 4;
	char param1 : 2;
	char param2 : 2;
} MemoryWord;


#endif