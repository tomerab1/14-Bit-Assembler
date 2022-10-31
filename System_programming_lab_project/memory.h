#ifndef MEMORY_H
#define MEMORY_H

/*
	This enumeration is used to represent each opcode with a specific numeric constant.
*/
typedef enum
{
	OP_MOV = 0, OP_CMP, OP_ADD, OP_SUB,
	OP_NOT, OP_CLR, OP_LEA, OP_INC,
	OP_DEC, OP_JMP, OP_BNE, OP_RED,
	OP_PRN, OP_JSR, OP_RTS, OP_STOP, OP_TOTAL
} Opcodes;

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