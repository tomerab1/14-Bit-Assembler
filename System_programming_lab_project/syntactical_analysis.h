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

typedef enum
{
	OP_TYPE_OPCDE, OP_TYPE_LABEL, OP_TYPE_DATA, OP_TYPE_STRING,
	OP_TYPE_EXTERN, OP_TYPE_ENTRY, OP_TYPE_UNKNOWN
} OperationTypes;

/* Division of operands to groups with the same syntax. */
typedef enum
{
	SG_GROUP_1, SG_GROUP_2, SG_GROUP_3, SG_GROUP_4, SG_GROUP_5,
	SG_GROUP_6, SG_GROUP_7, SG_GROUP_INVALID
} SyntaxGroups;

/* Forward decleration. */
typedef enum firstPassStates firstPassStates;

#define FLAG_NUMBER      1
#define FLAG_LABEL       2
#define FLAG_REGISTER    4
#define FLAG_PARAM_LABEL 8

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
bool cmp_register_name(const char* str);

/* This function trims the symbol name, i.e. trims the ':'
 * @param - The symbol to fix.
*/
void trim_symbol_name(char* sym);

/* Verifies the source code syntax */
bool validate_syntax(LineIterator it, firstPassStates state, long line, debugList* dbg_list);

/* Validates the syntax that may appear after a symbol definition. */
bool validate_syntax_sym_def(LineIterator* it, long line, debugList* dbg_list);



/* Verifies that the substring starting at word and ending at other is an integer. */
bool verify_int(LineIterator* it, long line, char* seps, debugList* dbg_list);

/* Matching syntax for each opcode type. */
bool match_syntax_group_1(LineIterator* it, long line, debugList* dbg_list);
bool match_syntax_group_2(LineIterator* it, long line, debugList* dbg_list);
bool match_syntax_group_3(LineIterator* it, long line, debugList* dbg_list);
bool match_syntax_group_4(LineIterator* it, long line, debugList* dbg_list);
bool match_syntax_group_5(LineIterator* it, long line, debugList* dbg_list);
bool match_syntax_group_6(LineIterator* it, long line, debugList* dbg_list);
bool match_syntax_group_7(LineIterator* it, long line, debugList* dbg_list);

bool match_syntax_opcode_dot_string(LineIterator* it, long line, debugList* dbg_list);
bool match_syntax_opcode_dot_data(LineIterator* it, long line, debugList* dbg_list);

bool is_register_name_heuristic(LineIterator it);
bool is_register_name(LineIterator* it);
bool is_label_name(LineIterator* it);

bool match_operand(LineIterator* it, long line, int flags, debugList* dbg_list);

SyntaxGroups get_syntax_group(const char* name);

bool recursive_match_pamaetrized_label(LineIterator* it, long line, debugList* dbg_list);

#endif