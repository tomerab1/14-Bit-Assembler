#ifndef ENCODING_H
#define ENCODING_H

#include "syntactical_analysis.h"
#include "line_iterator.h"
#include "debug.h"
#include "memory.h"

typedef struct
{ //should add type of var (#, register and so on)
	char* leftVar; /* Pointer to the start of the line. */
	EncodingTypes leftVarType;
	char* rightVar; /* Pointer to the current position in the line. */
	char* label;
} varData;

typedef enum { KIND_IMM, KIND_LABEL, KIND_LABEL_PARAM, KIND_REG, KIND_NONE } OperandKind;
typedef enum { ADDRESSING_IMM, ADDRESSING_DIR, ADDRESSING_PARAM, ADDRESSING_REG } AddressingType;

void encode_dot_string(LineIterator* it, memoryBuffer* img);
void encode_dot_data(LineIterator* it, memoryBuffer* img);
void encode_opcode(LineIterator* it, memoryBuffer* img);
void encode_integer(imageMemory* img, unsigned int num);
void encode_preceding_word(imageMemory* img, Opcodes op, char* source, char* dest, bool is_jump_label);
void encode_source_and_dest(imageMemory* img, char* source, char* dest);

/*groups: mov, add,sub,cmp,lea | expected input is the char next to end of command (i.e mov 
																						   ^ */
varData extract_variables_group_1_and_2_and_7(LineIterator* it, debugList* dbg_list);

/*groups: not, clr, inc, dec, red,prn | expected input is the char next to end of command (i.e mov
																								  ^ */
varData extract_variables_group_3_and_6(LineIterator* it, Opcodes command, debugList* dbg_list);

/*groups: jmp, bne, jsr | expected input is the char next to end of command (i.e mov
																				    ^ */
varData extract_variables_group_5(LineIterator* it, Opcodes command, debugList* dbg_list);


void encode_syntax_group_1(LineIterator* it, Opcodes op, memoryBuffer* img);
void encode_syntax_group_2(LineIterator* it, Opcodes op, memoryBuffer* img);
void encode_syntax_group_3(LineIterator* it, Opcodes op, memoryBuffer* img);
void encode_syntax_group_4(LineIterator* it, Opcodes op, memoryBuffer* img);
void encode_syntax_group_5(LineIterator* it, Opcodes op, memoryBuffer* img);
void encode_syntax_group_6(LineIterator* it, Opcodes op, memoryBuffer* img);
void encode_syntax_group_7(LineIterator* it, Opcodes op, memoryBuffer* img);

OperandKind get_operand_kind(char* op);


#endif