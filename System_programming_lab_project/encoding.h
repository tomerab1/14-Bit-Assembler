#ifndef ENCODING_H
#define ENCODING_H

/** @file
*/

#include "syntactical_analysis.h"
#include "line_iterator.h"
#include "debug.h"
#include "memory.h"

typedef struct
{ 
	char* leftVar; /* Pointer to the start of the line. */
	EncodingTypes leftVarEncType;
	char* rightVar; /* Pointer to the current position in the line. */
	EncodingTypes rightVarEncType;
	char* label;
	EncodingTypes labelEncType;
	int total;
} VarData;

typedef enum { REGISTER, LABEL, ASCII } VarType;

typedef enum { KIND_IMM, KIND_LABEL, KIND_LABEL_PARAM, KIND_REG, KIND_NONE } OperandKind;

typedef enum { ADDRESSING_IMM, ADDRESSING_DIR, ADDRESSING_PARAM, ADDRESSING_REG } AddressingType;

/**
* @brief Encode a dot string.
* @param it
* @param img
*/
void encode_dot_string(LineIterator* it, memoryBuffer* img);

/**
* @brief Encode the data section of a dot file.
* @param it
* @param img
*/
void encode_dot_data(LineIterator* it, memoryBuffer* img);

/*2 first digits are already encodede on first pass*/
/**
* @brief Encode the start of a label.
* @param it
* @param img
* @param symTable
* @param dbg_list Debug list to add debug info
*/
void encode_label_start_process(LineIterator* it, memoryBuffer* img, SymbolTable* symTable, debugList* dbg_list);

/**
* @brief Encode labels in an assembler image. This is a helper function.
* @param variables the data of the variables to encode. Must be sorted by leftVar and rightVar
* @param synGroup the syntax group for the assembler image.
* @param symTable the symbol table for the assembler image.
* @param img the assembler image to encode the labels into
*/
void encode_labels(VarData* variables, SyntaxGroups synGroup, SymbolTable* symTable, imageMemory* img);

/**
* @brief Encode a single opcode. This is the entry point for the encoding routines.
* @param it
* @param img
*/
void encode_opcode(LineIterator* it, memoryBuffer* img);

/**
* @brief Encode an integer in image memory. This is used to encode a variable - length integer ( 8 bits )
* @param img
* @param num the integer to be encoded in the image memory
*/
void encode_integer(imageMemory* img, unsigned int num);

/**
* @brief Encode a word preceding a label.
* @param img
* @param op The opcode of the instruction. Must be OP_PRECEDING or OP_JUMP_LABEL.
* @param source The source of the instruction or NULL if there is no source.
* @param dest The destination of the instruction or NULL if there is no destination.
* @param is_jmp_label Flag indicating whether or not this is a jump
*/
void encode_preceding_word(imageMemory* img, Opcodes op, char* source, char* dest, bool is_jmp_label);

/**
* @brief Encode source and destination registers. This is used to encode a pair of register or immediates.
* @param img
* @param source
* @param dest
*/
void encode_source_and_dest(imageMemory* img, char* source, char* dest);

/**
* @brief Extract variables from group 1 and 2 and 7 starting at the iterator.
* @param it Line iterator pointing to the start of the group
* @return A VarData structure with leftVar and rightVar set
* @Note groups: mov, add,sub,cmp,lea | expected input is the char next to end of command (i.e mov
																						   ^ 
*/
VarData extract_variables_group_1_and_2_and_7(LineIterator* it);

/**
* @brief Extract the variables group 3 and 6 from the line pointed to by it.
* @param it Line iterator pointing to the first word of the group
* @return a VarData structure with the leftVar and total
* @Note groups: not, clr, inc, dec, red,prn | expected input is the char next to end of command (i.e mov			  
*/
VarData extract_variables_group_3_and_6(LineIterator* it);

/**
* @brief Extract variables from GROUP 5 ( label =... ). This function assumes there is at least one token to be consumed.
* @param it Line iterator pointing to the start of the VARIABLES
* @return VarData with information about
*/
VarData extract_variables_group_5(LineIterator* it);

/**
* @brief Encode a syntax group 1 instruction. dependent syntax group that we use to encode source and destination operands.
* @param it Line iterator pointing to the start of the syntax group
* @param op Operand type to be encoded
* @param img
*/
void encode_syntax_group_1(LineIterator* it, Opcodes op, memoryBuffer* img);

/**
* @brief Encode a syntax group 2 instruction. This is the second part of the machine - dependent syntax group encoding.
* @param it Line iterator pointing to the start of the syntax group
* @param op Operand type to be used
* @param img
*/
void encode_syntax_group_2(LineIterator* it, Opcodes op, memoryBuffer* img);

/**
* @brief Encode a syntax group 3 instruction.
* @param it Line iterator pointing to the start of the instruction to encode.
* @param op The opcode of the instruction to encode. This is used to determine the source and destination word and to encode the source and destination respectively.
* @param img
*/
void encode_syntax_group_3(LineIterator* it, Opcodes op, memoryBuffer* img);

/**
* @brief Encode a syntax group 4. Encodes RTS and stop. This is used to encode the first memory word and the second and subsequent memory words
* @param it Line iterator pointing to the start of the line to be encoded.
* @param op The opcode of the encoding operation. This is always OP_SyntaxGroup4.
* @param img
*/
void encode_syntax_group_4(LineIterator* it, Opcodes op, memoryBuffer* img);

/*TBD will be discussed 03/01*/
/**
* @brief Encode a syntax group 5 instruction.
* @param it
* @param op The opcode of the encoding
* @param img
*/
void encode_syntax_group_5(LineIterator* it, Opcodes op, memoryBuffer* img);

/**
* @brief Encode a syntax group 6 instruction.
* @param it Line iterator pointing to the start of the line to be encoded.
* @param op Operand type to be used for encoding.
* @param img
*/
void encode_syntax_group_6(LineIterator* it, Opcodes op, memoryBuffer* img);

/**
* @brief Encode a syntax group 7 instruction.
* @param it Line iterator pointing to the start of the instruction
* @param op Operand type to be used for encoding
* @param img
*/
void encode_syntax_group_7(LineIterator* it, Opcodes op, memoryBuffer* img);

/**
* @brief Get the kind of operand.
* @param op
* @return KIND_NONE if op is NULL KIND_IMM if it is a hash_char KIND_REG if it is a
*/
OperandKind get_operand_kind(char* op);
#endif
