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

void encode_dot_string(LineIterator* it, memoryBuffer* img);
void encode_dot_data(LineIterator* it, memoryBuffer* img);
void encode_label_start_process(LineIterator* it, memoryBuffer* img, SymbolTable* symTable);
void encode_labels(VarData* variables, SyntaxGroups synGroup, SymbolTable* symTable, imageMemory* img);
void encode_opcode(LineIterator* it, memoryBuffer* img);
void encode_integer(imageMemory* img, unsigned int num);
void encode_preceding_word(imageMemory* img, Opcodes op, char* source, char* dest, bool is_jmp_label);
void encode_source_and_dest(imageMemory* img, char* source, char* dest);

/*groups: mov, add,sub,cmp,lea | expected input is the char next to end of command (i.e mov 
																						   ^ */
VarData extract_variables_group_1_and_2_and_7(LineIterator* it);

/*groups: not, clr, inc, dec, red,prn | expected input is the char next to end of command (i.e mov
																								  ^ */
VarData extract_variables_group_3_and_6(LineIterator* it);

/*groups: jmp, bne, jsr | expected input is the char next to end of command (i.e mov
																				    ^ */
VarData extract_variables_group_5(LineIterator* it);

/*opcodes: mov, add, sub*/
void encode_syntax_group_1(LineIterator* it, Opcodes op, memoryBuffer* img);

/*opcodes: cmp*/
void encode_syntax_group_2(LineIterator* it, Opcodes op, memoryBuffer* img);

/*opcodes: not, clr, inc,dec, red*/
void encode_syntax_group_3(LineIterator* it, Opcodes op, memoryBuffer* img);

/*opcodes: rts, stop*/
void encode_syntax_group_4(LineIterator* it, Opcodes op, memoryBuffer* img);

/*opcodes: jmp, bne, jsr*/
void encode_syntax_group_5(LineIterator* it, Opcodes op, memoryBuffer* img);

/*opcodes: prn*/
void encode_syntax_group_6(LineIterator* it, Opcodes op, memoryBuffer* img);

/*opcodes: lea*/
void encode_syntax_group_7(LineIterator* it, Opcodes op, memoryBuffer* img);

OperandKind get_operand_kind(char* op);
#endif
