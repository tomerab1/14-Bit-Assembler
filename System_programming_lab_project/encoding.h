#ifndef ENCODING_H
#define ENCODING_H

#include "syntactical_analysis.h"
#include "line_iterator.h"
#include "debug.h"
#include "memory.h"

typedef struct
{ //should add type of var (#, register and so on)
	char* leftVar; // Pointer to the start of the line.
	char* rightVar; // Pointer to the current position in the line.
	char* label;
} varData;

void encode_dot_string(LineIterator* it, memoryBuffer* img, debugList* dbg_list);

/*groups: mov, add,sub,cmp,lea | expected input is the char next to end of command (i.e mov 
																						   ^ */
varData extract_variables_group_1_and_2_and_7(LineIterator* it, debugList* dbg_list);

/*groups: not, clr, inc, dec, red,prn | expected input is the char next to end of command (i.e mov
																								  ^ */
varData extract_variables_group_3_and_6(LineIterator* it, Opcodes command, debugList* dbg_list);

/*groups: jmp, bne, jsr | expected input is the char next to end of command (i.e mov
																				    ^ */
varData extract_variables_group_5(LineIterator* it, Opcodes command, debugList* dbg_list);

#endif