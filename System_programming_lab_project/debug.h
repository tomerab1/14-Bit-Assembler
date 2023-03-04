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
	ERROR_CODE_LABEL_ALREADY_EXISTS_AS_EXTERN, ERROR_CODE_LABEL_ALREADY_EXISTS_AS_ENTRY,ERROR_CODE_LABEL_CANNOT_BE_DEFINED_AS_OPCODE_OR_REGISTER
} errorCodes;

/**
* A struct that represents an error context, i.e the line number of the error and error code.
* The 'start_pos' and 'end_pos' are used for formatting purposes.
*/
typedef struct
{
	char* start_pos;
	char* err_pos;
    ptrdiff_t err_len;
	long line_num;
	errorCodes err_code;
} errorContext;

typedef struct debug_node
{
	errorContext ctx;
	struct debug_node* next;
} debugNode;

typedef struct
{
	debugNode* head;
	debugNode* tail;
} debugList;

/**
* Registers a node at the end of the list.
* This is useful for adding nodes to the list in a way that is consistent with the order of the nodes in the list.
* @param list - The list.
* @param new_node - New error node.
*/
void debug_list_register_node(debugList* list, debugNode* new_node);

/**
* Returns true if the list is empty. This is the case if the head and tail are empty.
* @param list
* @return true if the list is empty false otherwise. 
*/
bool debug_list_is_empty(debugList* list);

/**
* Create a new list. The caller must free the returned list with debug_list_free ().
* @return A pointer to the new list. This should be freed by the caller after use of it as a pointer
*/
debugList* debug_list_new_list();

/**
* Create a new node and add it to the list. This is used when we want to build a list of debugNode's that are going to be printed to the console
* @param start
* @param err
* @param line - line number where the error occurred ( for debugging purposes )
* @param err_code - error code associated with the error ( for debugging purposes )
*
* @return pointer to the newly created node or NULL if there was insufficient memory to create it ( in which case the caller should free it
*/
debugNode* debug_list_new_node(char* start, char* err, long line, errorCodes err_code);

/**
* Destroys a node and frees all memory. This is useful for freeing an allocated node after it has been deallocated.
* @param node
*/
void debug_list_destroy_node(debugNode* node);

/**
* @brief Destroys a debugList and all its nodes.
* @param list A pointer to the debugList to be destroyed.
* @note After calling this function, the pointer to the debugList should be set to NULL.
* This function frees the memory allocated for a debugList and all its nodes.
* It first iterates over the linked list of debugNodes, and for each node,
* it calls debug_list_destroy_node to free any additional memory that was allocated for it,
* and then it frees the node itself.
* After all nodes are freed, it frees the memory allocated for the debugList itself.
@see debug_list_destroy_node()
*/
void debug_list_destroy(debugList** list);

/**
* Prints a list of debugNode to stderr. This is useful for debugging the error messages that are sent to the user
* @param list
*/
void debug_list_pretty_print(debugList* list);

/**
* @brief Map a token code to a human readable error. This is used to display errors in debug output
* @param code the token code to map
* @return pointer to the error string or NULL if none could be
*/
char* debug_map_token_to_err(errorCodes code);

/**
* Prints an error to stderr. This is used for debugging and should not be called externally. The err_ctx and err_buff are passed by reference so it can be manipulated by the caller.
*
* @param err_ctx
* @param err_buff
*/
void debug_print_error(errorContext* err_ctx, char err_buff[]);

#endif