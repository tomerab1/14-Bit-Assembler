#ifndef  DEBUG_H
#define DEBUG_H

/** @file
*/

#include "utils.h"
#include <stddef.h>

/**
 * @brief Error codes used in debug_map_token_to_err()
*/
typedef enum
{
	ERROR_CODE_OK, ERROR_CODE_UNKNOWN, ERROR_CODE_INVALID_NAME, ERROR_CODE_TO_MANY_OPERANDS, 
	ERROR_CODE_TO_LITTLE_OPERANDS, ERROR_CODE_SYNTAX_ERROR, ERROR_CODE_SYMBOL_REDEFINITION,
	ERROR_CODE_INVALID_SYM_TYPE, ERROR_CODE_SPACE_BEFORE_COLON, ERROR_CODE_INVALID_CHAR_IN_LABEL,
	ERROR_CODE_RESERVED_KEYWORD_DEF, ERROR_CODE_SYMBOL_IGNORED_WARN, ERROR_CODE_INVALID_AMOUNT_OF_OPERANDS,
	ERROR_CODE_COMMA_AFTER_INSTRUCTION, ERROR_CODE_MISSING_COMMA, ERROR_CODE_VALUE_ERROR, ERROR_CODE_EXTRA_COMMA,
	ERROR_CODE_INVALID_LABEL_DEF, ERROR_CODE_MISSING_OPEN_PAREN, ERROR_CODE_INVALID_COMMA_POS, ERROR_CODE_INVALID_INT,
	ERROR_CODE_SPACE_AFTER_OPERAND, ERROR_CODE_INVALID_OPERAND, ERROR_CODE_INVALID_WHITE_SPACE, ERROR_CODE_EXTRA_PAREN,
	ERROR_CODE_MISSING_OPEN_QUOTES, ERROR_CODE_MISSING_CLOSE_QUOTES, ERROR_CODE_TEXT_AFTER_END, ERROR_CODE_MISSING_OPERAND,ERROR_CODE_LABEL_DOES_NOT_EXISTS,
	ERROR_CODE_LABEL_ALREADY_EXISTS_AS_EXTERN, ERROR_CODE_LABEL_ALREADY_EXISTS_AS_ENTRY, ERROR_CODE_LABEL_CANNOT_BE_DEFINED_AS_OPCODE_OR_REGISTER, ERROR_CODE_LABEL_MISSING_OR_NON_EXISTS_OPCODE
} errorCodes;

/**
 * @brief Represents a context for an error in the assembly code.
 */
typedef struct errorContext errorContext;

/**
* @brief Map a token code to a human readable error. This is used to display errors in debug output
* @param code the token code to map
* @return pointer to the error string or NULL if none could be
*/
char* map_token_to_err(errorCodes code);

/**
* Prints an error to stderr. This is used for debugging and should not be called externally. The err_ctx and err_buff are passed by reference so it can be manipulated by the caller.
*
* @param err_ctx
* @param err_buff
*/
void print_error(errorContext* err_ctx);

#endif