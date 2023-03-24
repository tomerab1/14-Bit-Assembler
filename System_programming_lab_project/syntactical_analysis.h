#ifndef SYNTACTICAL_ANALYSIS_H
#define SYNTACTICAL_ANALYSIS_H

/** @file
*/

#include "first_pass.h"

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
	SG_GROUP_1 = 0, SG_GROUP_2, SG_GROUP_3, SG_GROUP_4, SG_GROUP_5,
	SG_GROUP_6, SG_GROUP_7, SG_TOTAL, SG_GROUP_INVALID
} SyntaxGroups;

#define FLAG_NUMBER      1
#define FLAG_LABEL       2
#define FLAG_REGISTER    4
#define FLAG_PARAM_LABEL 8

/**
* @brief Get opcode from string. This is used to parse opcodes that are passed to libc functions.
*
* @param str
*
* @return OpCode corresponding to str or 0 if str doesn't match any opcode
*/
Opcodes get_opcode(char* str);

/** 
 * @brief This function checks the lable syntax.
 * @param - The label to do the syntax check upon.  
 * @return - An appropriate error code, if no errors found, returns ERROR_CODE_OK.
 */
errorCodes check_label_syntax(char* label);

/** 
 * @brief Checks if the label syntax is valid.
 * @param - The label string.
 * @return - TRUE if valid, FALSE otherwise.
*/
bool is_valid_label(char* label);

/**
* @brief Check if the next word is a label. This is used to detect whether or not we are in a label context.
*
* @param line
*
* @return @c true if the next word is a label @c false otherwise
*/
bool isLabel(LineIterator* line);

/**
* @brief Verify the syntax of a command. This is a helper function for debug_command ().
*
* @param it - LineIterator to use for verification. It is assumed that the parser has already advanced through the line that is being processed.
* @param dbg_list - The debugList to which the command belongs.
*
* @return TRUE if the syntax is correct FALSE otherwise. In this case the parser should be instructed to abort
*/
bool verify_command_syntax(LineIterator* it);


/**
* @brief Compare register name to known ones. This is used to determine if we are interested in a register
*
* @param str
*
* @return TRUE if name is known else FALSE ( case insensitive
*/
bool cmp_register_name(char* str);

/**
* @brief Check if the next character is a label name. This is used to distinguish between labels that are separated by spaces and blank lines.
*
* @param it
*
* @return TRUE if the next character is a label name FALSE otherwise
*/
bool is_label_name(LineIterator* it);

/**
* @brief Trim the symbol name to make it easier to read. This is used to prevent accidental redefinitions of symbols that are in the middle of a function name.
*
* @param sym
*/
void trim_symbol_name(char* sym);

/**
* @brief Checks if name is a reserved keyword.
*
* @param name
* @return True if it is a reserved keyword, false otherwise.
*/
bool is_reserved_word(char* name);

/**
* @brief Validate the syntax at the current position. This is a wrapper around validate_syntax_opcode () validate_syntax_data () and validate_syntax_string () to allow custom code to be added to the debugList
*
* @param it - Iterator over the line to be validated
* @param state - The first pass state of the syntax
* @param line - The line number at which the syntax is being validated
* @param dbg_list - The list of debuggers to which warnings / errors are added
*
* @return TRUE if the syntax is valid FALSE if it is
*/
bool validate_syntax(LineIterator it, firstPassStates state, long line);

/**
* @brief Validate a syntax opcode. This is a helper function for validate_opcode ().
*
* @param it
* @param line - Line number on which the parser was started.
* @param dbg_list - Debug list to add warnings / errors to.
*
* @return true if the opcode is valid false otherwise. Note that it is up to the caller to free it
*/
bool validate_syntax_opcode(LineIterator* it, long line);

/**
* @brief Verifies that the next character is a valid integer. This is done by examining the characters that begin with a digit followed by a comma or a comma and verifying that it is a valid integer.
*
* @param it
* @param line - Line in which the string was read. Used for error reporting.
* @param seps - String of digits to be checked for validity.
* @param dbg_list - Debug list to be populated with nodes.
*
* @return TRUE if the integer is valid FALSE otherwise. On failure the iterator is left pointing at the character that failed
*/
bool verify_int(LineIterator* it, long line, char* seps);

/**
* @brief This function reduce repetative code in the match_syntax_group_1 and match_syntax_group_2 functions.
* It checks if their operands are valid.
* @return True if valid, false otherwise.
*/
bool match_operands_for_sg_1_2(LineIterator* it, long line);

/**
* @brief Matches a syntax group 1 & 2. This is the first part of the syntax group : it must be at the start of a variable or variable declaration.
*
* @param it
* @param line - The current line number being matched. Used for error reporting.
* @param dbg_list - The debug list to add debug nodes to.
*
* @return TRUE if a match was found FALSE otherwise. The match is terminated by a newline
*/
bool match_syntax_group_1_2(LineIterator* it, long line);

/**
* @brief Matches a syntax group 3. This is the third part of the syntax group matching algorithm.
*
* @param it
* @param line - Line number being matched. Used for error reporting.
* @param dbg_list - Debug list for the matched nodes.
*
* @return TRUE if a match was found FALSE otherwise. When false the iterator is left pointing at the character immediately following the end of the syntax group
*/
bool match_syntax_group_3(LineIterator* it, long line);

/**
* @brief Matches a syntax group 4. This is used to check if there is a'%'followed by an operand.
*
* @param it
* @param line - Line number where the token begins. This is used for reporting errors.
* @param dbg_list - Debug list to add matched nodes to.
*
* @return TRUE if a match was found FALSE otherwise. If FALSE is returned the iterator is not advanced
*/
bool match_syntax_group_4(LineIterator* it, long line);

/**
* @brief Matches a syntax group 5 starting at the current position. This is used to implement the syntax group 5 ( and below ) as well as the syntax group 5 ( and below ).
*
* @param it
* @param line - Line number of the current position
* @param dbg_list - Debug list for the matched nodes
*
* @return TRUE if a match was found FALSE if not ( or an error occurred
*/
bool match_syntax_group_5(LineIterator* it, long line);

/**
* @brief Matches a syntax group 6 starting at the current position. This is used for variable and variable references.
*
* @param it
* @param line - Line in the source file being scanned
* @param dbg_list - Debug list for the matches.
*
* @return TRUE if a match was found FALSE otherwise ( an error has been reported
*/
bool match_syntax_group_6(LineIterator* it, long line);

/**
* @brief Matches a syntax group 7. In this case we are interested in the name of a register or a label.
*
* @param it
* @param line - Line number where the syntax group starts. Used for error reporting.
* @param dbg_list - Debug list for all matches.
*
* @return TRUE if a match was found FALSE otherwise. This function is called by match_syntax_group ()
*/
bool match_syntax_group_7(LineIterator* it, long line);

/**
* @brief Validates a syntax string.
*
* @param it
* @param line - Line number of the string being validated
* @param dbg_list - Debug list to add nodes to
*
* @return TRUE if the string is valid FALSE if it is isn't
*/
bool validate_syntax_string(LineIterator* it, long line);

/**
* @brief Validates a syntax data. 
*
* @param it
* @param line - Line number of the string being validated
* @param dbg_list - Debug list to add nodes to
*
* @return TRUE if the data is valid FALSE if it isn't
*/
bool validate_syntax_data(LineIterator* it, long line);

/**
* @brief Validates the syntax extern and entry. This is used to validate the syntax extern and entry in a macro expansion.
*
* @param it
* @param line - Line number of the comment
* @param dbg_list
*
* @return TRUE if there is at least one error FALSE if isn't
*/
bool validate_syntax_extern_and_entry(LineIterator* it, long line);


/**
* @brief Check if the next character is a register name.
*
* @param it
*
* @return True if the next character is a register name false otherwise
*/
bool is_register_name(LineIterator* it);

/**
* @brief Validate that the iterator is at the end of a label. This is used to determine whether or not we should stop at the end of an already - processed label or not.
*
* @param it
*
* @return true if \ a it is at the end of a label false otherwise
*/
bool validate_label_ending(LineIterator* it);

/**
* @brief Matches an operand and returns TRUE if it matches. This is a low - level function used by match_operand_p () and match_operand_p2 ().
*
* @param it
* @param line - Line number of the operand. This is used for error reporting.
* @param flags - Flags to match the operand with. Flags can be FLAG_LABEL FLAG_NUMBER FLAG_REAL_REG and FLAG_REG_NEAREST
* @param dbg_list - Debug list to fill with nodes.
*
* @return TRUE if the operand matches FALSE otherwise. On failure the iterator is left pointing at the end of the operand
*/
bool match_operand(LineIterator* it, long line, int flags);

/**
* @brief Get the syntax group of a name. This is used to distinguish groups of syntaxes that are different from each other.
*
* @param name
*
* @return @c SG_GROUP_1 if the name is a syntax group @c SG_GROUP_2
*/
SyntaxGroups get_syntax_group(char* name);

/**
* @brief Matches pamaetrized labels. This is a bit tricky because we don't know the number of arguments to a label.
*
* @param it
* @param line - Line number where the pattern was found. Used for error reporting.
* @param dbg_list - Debug list to add warnings to.
*
* @return TRUE if it succeeds FALSE otherwise. Note that it does not check for syntax errors
*/
bool match_pamaetrized_label(LineIterator* it, long line);

/**
* @brief Check if we are looking for a heuristics to be used in register names.
*
* @param it - Iterator on the line to check. Must be at the start of a register name.
*
* @return true if it is false otherwise. This is a heuristic
*/
bool is_register_name_heuristic(LineIterator it);

/**
* @brief Checks if a directive exists. This is a basic check that doesn't look for ". " and it's the only way to check for it.
* @param line
* @return TRUE if there is a directive FALSE otherwise. The iterator is advanced past the directive
*/

bool is_register_name_whole(LineIterator* it);

/**
@brief Checks if a directive exists in the given line.
@param line The line to check for a directive.
@return Returns true if the line contains a directive, otherwise false.
This function checks if a directive exists in the given line. The supported directives are ".string",
".data", ".extern", and ".entry". It uses the line_iterator_word_includes function to determine if
the line contains any of these directives.
*/
bool directive_exists(LineIterator* line);
#endif
