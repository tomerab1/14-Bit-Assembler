#ifndef SYNTACTICAL_ANALYSIS_H
#define SYNTACTICAL_ANALYSIS_H

#include "line_iterator.h"
#include "debug.h"
#include "utils.h"

/*
	This enumeration is used to represent each opcode with a specific numeric constant.
*/
typedef enum
{
	OP_MOV = 0, OP_CMP, OP_ADD, OP_SUB,
	OP_NOT, OP_CLR, OP_LEA, OP_INC,
	OP_DEC, OP_JMP, OP_BNE, OP_RED,
	OP_PRN, OP_JSR, OP_RTS, OP_STOP, OP_UNKNOWN
} Opcodes;

/*
	The instructions group can be divided to three groups,
	those taking 1 operand, 2 operands or no operands
*/
typedef enum
{
	IG_GROUP_0, IG_GROUP_1, IG_GROUP_2, IG_GROUP_INVALID
} InstructionGroup;

typedef enum
{
	AG_GROUP_0, AG_GROUP_1, AG_GROUP_2, AG_GROUP_3, AG_GROUP_UNKNOWN
} AddressingGroups;

typedef enum
{
	OP_TYPE_OPCDE, OP_TYPE_LABEL, OP_TYPE_DATA, OP_TYPE_STRING,
	OP_TYPE_EXTERN, OP_TYPE_ENTRY, OP_TYPE_UNKNOWN
} OperationTypes;

/* Forward decleration. */
typedef enum firstPassStates firstPassStates;


/* This function takes a string and returns the matching Opcode. */
Opcodes get_opcode(const char* str);

/* This function checks the lable syntax.
 * @param - The label to do the syntax check upon.  
 * @return - An appropriate error code, if no errors found, returns ERROR_CODE_OK.
 */
errorCodes check_label_syntax(const char* label);

/* Checks if the label syntax is valid.
 * @param - The label string.
 * @return - TRUE if valid, FALSE otherwise.
*/
bool is_valid_label(const char* label);

/* This function verifies the command syntax. */
bool verify_command_syntax(LineIterator* it, debugList* dbg_list);

/* Check if 'str' is a registers name. */
bool is_register_name(const char* str);

/* This function trims the symbol name, i.e. trims the ':'
 * @param - The symbol to fix.
*/
void trim_symbol_name(char* sym);

/* Verifies the source code syntax */
bool validate_syntax(LineIterator it, firstPassStates state, long line, debugList* dbg_list);

/* Validates the syntax that may appear after a symbol definition. */
bool validate_syntax_sym_def(LineIterator* it, long line, debugList* dbg_list);

/* Returns the instruction group of 'str'. */
InstructionGroup get_instruction_group(const char* str);

/* Verifies syntax for the group of operation that can take 0 operands.
*/
bool check_syntax_group_zero(LineIterator* it, long line, debugList* dbg_list);


/* Verifies syntax for the group of operation that can take 1 operands.
*/
bool check_syntax_group_one(LineIterator* it, long line, Opcodes opcode, debugList* dbg_list);


/* Verifies syntax for the group of operation that can take 2 operands.
*/
bool check_syntax_group_two(LineIterator* it, long line, Opcodes opcode, debugList* dbg_list);

/* Checks the syntax according to the addressing group, in page 32 (PDF).*/
bool match_addressing_group_zero(LineIterator* it, long line, debugList* dbg_list);
bool match_addressing_group_one(LineIterator* it, long line, debugList* dbg_list);
bool match_addressing_group_two(LineIterator* it, long line, debugList* dbg_list);
bool match_addressing_group_three(LineIterator* it, long line, debugList* dbg_list);

/* Verifies that the substring starting at word and ending at other is an integer. */
bool verify_int(LineIterator* it, long line, char* seps, debugList* dbg_list);

AddressingGroups classify_to_addressing_group(const char* word);

/* Boolean function for classifying to each addressing group */
bool is_matching_adressing_group_zero(const char* word);
bool is_matching_adressing_group_one(const char* word);
bool is_matching_adressing_group_two(const char* word);
bool is_matching_adressing_group_three(const char* word);

bool recursive_match_addressing_group_two(LineIterator* it, long line, debugList* dbg_list);

#endif