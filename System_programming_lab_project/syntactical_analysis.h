#ifndef SYNTACTICAL_ANALYSIS_H
#define SYNTACTICAL_ANALYSIS_H

/** @file
*/

#include "first_pass.h"
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
	DOT_DATA_CODE = 1,
	DOT_STRING_CODE,
	DOT_ENTRY_CODE,
	DOT_EXTERN_CODE
} ENUM_1;

typedef enum
{
	EMPTY_SENTENCE = 1,
	COMMENT_SENTENCE,
	DIRECTIVE_SENTENCE,
	INSTRUCTION_SENTENCE
} ENUM_2;

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

#define FLAG_NUMBER      1
#define FLAG_LABEL       2
#define FLAG_REGISTER    4
#define FLAG_PARAM_LABEL 8

/* This function takes a string and returns the matching Opcode. */
Opcodes get_opcode(char* str);

/** This function checks the lable syntax.
 * @param - The label to do the syntax check upon.  
 * @return - An appropriate error code, if no errors found, returns ERROR_CODE_OK.
 */
errorCodes check_label_syntax(char* label);

/** Checks if the label syntax is valid.
 * @param - The label string.
 * @return - TRUE if valid, FALSE otherwise.
*/
bool is_valid_label(char* label);

/*checks if the first word is a label, used during second pass*/
bool isLabel(LineIterator* line);

/* This function verifies the command syntax. */
bool verify_command_syntax(LineIterator* it, debugList* dbg_list);

/* Check if 'str' is a registers name. */
bool cmp_register_name(char* str);

/** This function trims the symbol name, i.e. trims the ':'
 * @param - The symbol to fix.
*/
void trim_symbol_name(char* sym);

/* Verifies the source code syntax */
bool validate_syntax(LineIterator it, firstPassStates state, long line, debugList* dbg_list);

/* Validates the syntax that may appear after a symbol definition. */
bool validate_syntax_opcode(LineIterator* it, long line, debugList* dbg_list);

/* Verifies that the substring starting at word and ending at other is an integer. */
bool verify_int(LineIterator* it, long line, char* seps, debugList* dbg_list);

/* Matching syntax for each opcode type. */
/*groups: mov, add,sub */
bool match_syntax_group_1(LineIterator* it, long line, debugList* dbg_list);
/*groups: cmp */
bool match_syntax_group_2(LineIterator* it, long line, debugList* dbg_list);
/*groups: not, clr, inc, dec, red */
bool match_syntax_group_3(LineIterator* it, long line, debugList* dbg_list);
/*groups: rts,stop */
bool match_syntax_group_4(LineIterator* it, long line, debugList* dbg_list);
/*groups: jmp, bne, jsr */
bool match_syntax_group_5(LineIterator* it, long line, debugList* dbg_list);
/*groups: prn */
bool match_syntax_group_6(LineIterator* it, long line, debugList* dbg_list);
/*groups: lea */
bool match_syntax_group_7(LineIterator* it, long line, debugList* dbg_list);

bool validate_syntax_string(LineIterator* it, long line, debugList* dbg_list);
bool validate_syntax_data(LineIterator* it, long line, debugList* dbg_list);
bool validate_syntax_extern_and_entry(LineIterator* it, long line, debugList* dbg_list);

bool is_register_name_heuristic(LineIterator it);
bool is_register_name(LineIterator* it);
bool is_label_name(LineIterator* it);
bool validate_label_ending(LineIterator* it);

bool match_operand(LineIterator* it, long line, int flags, debugList* dbg_list);

SyntaxGroups get_syntax_group(char* name);

bool match_pamaetrized_label(LineIterator* it, long line, debugList* dbg_list);

/*param - line iterator pointed to start | return sentence type (page 24), returns empty = 1, comment = 2, directive = 3, instruction =4*/
int extract_sentence_type(LineIterator* it);

/*returns true/false if directive(.something) exists, doesn't ensure propriety of order and/or order type*/
bool directive_exists_basic(LineIterator* line);

/*returns true/false if instruction(any of opcode) exists*/
bool find_if_instruction_exists(LineIterator* line);

/*skips label and and consume blanks if exists after the label*/
void skip_label_basic(LineIterator* line);

#endif