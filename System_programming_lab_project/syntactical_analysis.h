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
	DOT_DATA_CODE = 1,
	DOT_STRING_CODE,
	DOT_ENTRY_CODE,
	DOT_EXTERN_CODE
};

typedef enum
{
	EMPTY_SENTENCE = 1,
	COMMENT_SENTENCE,
	DIRECTIVE_SENTENCE,
	INSTRUCTION_SENTENCE
};


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

/*checks if the first word is a label, used during second pass*/
bool isLabel(LineIterator* line);

/* This function verifies the command syntax. */
bool verify_command_syntax(LineIterator* it, debugList* dbg_list);

/* Check if 'str' is a registers name. */
bool is_register_name(const char* str);

/* This function trims the symbol name, i.e. trims the ':'
 * @param - The symbol to fix.
*/
void trim_symbol_name(char* sym);

/* Verifies the source code syntax */
bool validate_syntax(LineIterator it, debugList* dbg_list);

/*param - line iterator pointed to start | return sentence type (page 24), returns empty = 1, comment = 2, directive = 3, instruction =4*/
int extract_sentence_type(LineIterator* it);

/*returns true/false if directive(.something) exists, doesn't ensure propriety of order and/or order type*/
bool directive_exists_basic(LineIterator* line);
#endif