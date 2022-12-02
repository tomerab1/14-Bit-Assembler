#ifndef  DEBUG_H
#define DEBUG_H
#include "utils.h"

typedef enum
{
	ERROR_CODE_OK, ERROR_CODE_UNKNOWN, ERROR_CODE_INVALID_NAME, ERROR_CODE_TO_MANY_OPERANDS, 
	ERROR_CODE_TO_LITTLE_OPERANDS, ERROR_CODE_SYNTAX_ERROR, ERROR_CODE_SYMBOL_REDEFINITION,
	ERROR_CODE_INVALID_SYM_TYPE, ERROR_CODE_SPACE_BEFORE_COLON, ERROR_CODE_INVALID_CHAR_IN_LABEL,
	ERROR_CODE_RESERVED_KEYWORD_DEF, ERROR_CODE_SYMBOL_IGNORED_WARN, ERROR_CODE_INVALID_AMOUNT_OF_OPERANDS,
	ERROR_CODE_COMMA_AFTER_INSTRUCTION, ERROR_CODE_MISSING_COMMA, ERROR_CODE_VALUE_ERROR, ERROR_CODE_EXTRA_COMMA,
	ERROR_CODE_INVALID_LABEL_DEF
} errorCodes;

/*
	A struct that represents an error context, i.e the line number of the error and error code.
	The 'start_pos' and 'end_pos' are used for formatting purposes.
*/
typedef struct
{
	char* start_pos;
	char* err_pos;
	int line_num;
	errorCodes err_code;
} errorContext;

typedef struct __debug_node
{
	errorContext ctx;
	struct __debug_node* next;
} debugNode;

typedef struct
{
	debugNode* head;
	debugNode* tail;
} debugList;

/*
	Function for inserting nodes to the list.
*/
void debug_list_register_node(debugList* list, debugNode* new_node);

/*
	Function for checking if the list is empty.
*/
bool debug_list_is_empty(debugList* list);

/*
	Function for creating a new list.
*/
debugList* debug_list_new_list();

/*
	Function for creating a new debugNode
*/
debugNode* debug_list_new_node(char* start, char* err, int line, errorCodes err_code);

/*
	Function for freeing a debugList node
*/
void debug_list_destroy_node(debugNode* node);

/*
	Function for freeing the list.
*/
void debug_list_destroy(debugList** list);

/*
	Function for printing pretty errors.
*/
void debug_list_pretty_print(debugList* list);

/*
	Function for mapping a errorCode to a string
*/

const char* debug_map_token_to_err(errorCodes code);

/* Function for printing the errors
	
*/
void debug_print_error(errorContext* err_ctx, char err_buff[]);

#endif