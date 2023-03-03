/** @file
*/

#include "debug.h"
#include <string.h>


/**
* Registers a node at the end of the list. This is useful for adding nodes to the list in a way that is consistent with the order of the nodes in the list.
* 
* @param list
* @param new_node
*/
void debug_list_register_node(debugList* list, debugNode* new_node)
{
	if (debug_list_is_empty(list)) {
		list->head = list->tail = new_node;
	}
	else {
		list->tail->next = new_node;
		list->tail = list->tail->next;
	}
}

/**
* Returns true if the list is empty. This is the case if the head and tail are empty.
* 
* @param list
* 
* @return true if the list is empty false otherwise. Note that this does not mean a valid list but it does mean that there are no elements in the list
*/
bool debug_list_is_empty(debugList* list)
{
	return !list->head && !list->tail;
}

/**
* Create a new list. The caller must free the returned list with debug_list_free ().
* 
* 
* @return A pointer to the new list. This should be freed by the caller after use of it as a pointer
*/
debugList* debug_list_new_list()
{
	debugList* new_list = (debugList*)xmalloc(sizeof(debugList));
	new_list->head = new_list->tail = NULL;
	return new_list;
}

/**
* Create a new node and add it to the list. This is used when we want to build a list of debugNode's that are going to be printed to the console
* 
* @param start
* @param err
* @param line - line number where the error occurred ( for debugging purposes )
* @param err_code - error code associated with the error ( for debugging purposes )
* 
* @return pointer to the newly created node or NULL if there was insufficient memory to create it ( in which case the caller should free it
*/
debugNode* debug_list_new_node(char* start, char* err, long line, errorCodes err_code)
{
	debugNode* new_node = (debugNode*)xmalloc(sizeof(debugNode));
	/* The node does not own the start and err pointers, we should not call 'free' on them ! */
	new_node->ctx.start_pos = get_copy_string(start);
	new_node->ctx.err_pos = get_copy_string(err);
    new_node->ctx.err_len = (err - start);
	new_node->ctx.line_num = line;
	new_node->ctx.err_code = err_code;
	new_node->next = NULL;
	return new_node;
}

/**
* Destroys a node and frees all memory. This is useful for freeing an allocated node after it has been deallocated.
* 
* @param node
*/
void debug_list_destroy_node(debugNode* node)
{
	free(node->ctx.start_pos);
	free(node->ctx.err_pos);
}

/**
* Destroys a list and all its elements. This is useful for freeing memory allocated by debug_list_init ().
* 
* @param list
*/
void debug_list_destroy(debugList** list)
{
	debugNode* head = (*list)->head, *next;

	while(head) {
		next = head->next;
		debug_list_destroy_node(head);
		free(head);
		head = next;
	}

	free(*list);
}

/**
* Prints a list of debugNode to stderr. This is useful for debugging the error messages that are sent to the user
* 
* @param list
*/
void debug_list_pretty_print(debugList* list)
{
	char err_buff[80] = { 0 };

	LIST_FOR_EACH(debugNode, list->head, head) {
		switch (head->ctx.err_code) {
		default:
			debug_print_error(&head->ctx, err_buff);
			break;
		}
	}
}

/**
* Print an error to stderr. This is used for debugging and should not be called externally. The err_ctx and err_buff are passed by reference so it can be manipulated by the caller.
* 
* @param err_ctx
* @param err_buff
*/
void debug_print_error(errorContext* err_ctx, char err_buff[])
{
	/* Calculate the spacing between the start of the line and the error pos. */
	sprintf(err_buff, "Line %li:", err_ctx->line_num);
	ptrdiff_t offset = (int) strlen(err_buff) + err_ctx->err_len + 1;

	printf("%s %s\n", err_buff, err_ctx->start_pos);
	while (offset > 0) {
		printf(" ");
		offset--;
	}
	printf("^\t\nError: %s\n\n", debug_map_token_to_err(err_ctx->err_code));
}

/**
* Map a token to a human readable error. This is used to print error messages in debug output.
* 
* @param code - the code of the token to map. Must be one of the ERROR_CODE_ * values
* 
* @return pointer to a string that will be used as the
*/
char* debug_map_token_to_err(errorCodes code)
{
	switch (code) {
	case ERROR_CODE_TO_MANY_OPERANDS: return "To many operands";
	case ERROR_CODE_SYNTAX_ERROR: return "Syntax error";
	case ERROR_CODE_EXTRA_COMMA: return "Extra comma";
    case ERROR_CODE_SYMBOL_REDEFINITION: return "Symbol redefinition";
	case ERROR_CODE_LABEL_DOES_NOT_EXISTS: return "Could not find Label in symbol map.";
	case ERROR_CODE_LABEL_ALREADY_EXISTS_AS_ENTRY: return "Label already defined as entry.";
	case ERROR_CODE_LABEL_ALREADY_EXISTS_AS_EXTERN: return "Label already defined as extern.";
	case ERROR_CODE_LABEL_CANNOT_BE_DEFINED_AS_OPCODE_OR_REGISTER: return "Label cannot be defined as Opcode or Register";

	default:
		return "UnknownError";
	}
}
