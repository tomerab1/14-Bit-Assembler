#include "debug.h"
#include <string.h>

/**
 *The `errorContext` struct contains information about the position of an error in the
 * assembly code, including the start position, the position of the error, the length of
 * the error, the line number, and an error code indicating the type of error.
*/
struct errorContext
{
	char* start_pos;
	char* err_pos;
	ptrdiff_t err_len;
	long line_num;
	errorCodes err_code;
};

/**
 * The `debugNode` struct contains an `errorContext` struct representing debugging
 * information for the assembly code, as well as a pointer to the next node in the linked
 * list.
 */
struct debugNode
{
	errorContext ctx;
	struct debugNode* next;
};

/**
 * The `debugList` struct contains pointers to the head and tail nodes of a linked list of
 * `debugNode` structs, which contain debugging information for the assembly code.
*/
struct debugList
{
	debugNode* head;
	debugNode* tail;
};

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

bool debug_list_is_empty(debugList* list)
{
	return !list->head && !list->tail;
}

debugList* debug_list_new_list()
{
	debugList* new_list = (debugList*)xmalloc(sizeof(debugList));
	new_list->head = new_list->tail = NULL;
	return new_list;
}

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

void debug_list_destroy_node(debugNode* node)
{
	free(node->ctx.start_pos);
	free(node->ctx.err_pos);
}

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

void debug_list_pretty_print(debugList* list)
{
	char err_buff[DEBUG_LINE_MAX_LENGTH] = { 0 };

	LIST_FOR_EACH(debugNode, list->head, head) {
		switch (head->ctx.err_code) {
		default:
			debug_print_error(&head->ctx, err_buff);
			break;
		}
	}
}

void debug_print_error(errorContext* err_ctx, char err_buff[])
{
	ptrdiff_t offset;

	/* Calculate the spacing between the start of the line and the error pos. */
	sprintf(err_buff, "Line %li:", err_ctx->line_num);
	offset = (int) strlen(err_buff) + err_ctx->err_len + 1;

	printf("%s %s\n", err_buff, err_ctx->start_pos);
	while (offset > 0) {
		printf(SPACE_STRING);
		offset--;
	}
	printf("^\t\nError: %s\n\n", debug_map_token_to_err(err_ctx->err_code));
}

char* debug_map_token_to_err(errorCodes code)
{
	switch (code) {
	case ERROR_CODE_OK: return "No errors";
	case ERROR_CODE_UNKNOWN: return "Unknown error";
	case ERROR_CODE_INVALID_NAME: return "Invalid name";
	case ERROR_CODE_TO_MANY_OPERANDS: return "Too many operands";
	case ERROR_CODE_TO_LITTLE_OPERANDS: return "Too few operands";
	case ERROR_CODE_SYNTAX_ERROR: return "Syntax error";
	case ERROR_CODE_SYMBOL_REDEFINITION: return "Symbol redefinition";
	case ERROR_CODE_INVALID_SYM_TYPE: return "Invalid symbol type";
	case ERROR_CODE_SPACE_BEFORE_COLON: return "Space before colon";
	case ERROR_CODE_INVALID_CHAR_IN_LABEL: return "Invalid character in label";
	case ERROR_CODE_RESERVED_KEYWORD_DEF: return "Reserved keyword definition";
	case ERROR_CODE_SYMBOL_IGNORED_WARN: return "Symbol ignored (warning)";
	case ERROR_CODE_INVALID_AMOUNT_OF_OPERANDS: return "Invalid amount of operands";
	case ERROR_CODE_COMMA_AFTER_INSTRUCTION: return "Comma after instruction";
	case ERROR_CODE_MISSING_COMMA: return "Missing comma";
	case ERROR_CODE_VALUE_ERROR: return "Value error";
	case ERROR_CODE_EXTRA_COMMA: return "Extra comma";
	case ERROR_CODE_INVALID_LABEL_DEF: return "Invalid label definition";
	case ERROR_CODE_MISSING_OPEN_PAREN: return "Missing open parenthesis";
	case ERROR_CODE_INVALID_COMMA_POS: return "Invalid comma position";
	case ERROR_CODE_INVALID_INT: return "Invalid integer";
	case ERROR_CODE_SPACE_AFTER_OPERAND: return "Space after operand";
	case ERROR_CODE_INVALID_OPERAND: return "Invalid or couldn't find operand";
	case ERROR_CODE_INVALID_WHITE_SPACE: return "Invalid white space";
	case ERROR_CODE_EXTRA_PAREN: return "Extra parenthesis";
	case ERROR_CODE_MISSING_OPEN_QUOTES: return "Missing open quotes";
	case ERROR_CODE_MISSING_CLOSE_QUOTES: return "Missing close quotes";
	case ERROR_CODE_TEXT_AFTER_END: return "Text after end directive";
	case ERROR_CODE_MISSING_OPERAND: return "Missing operand";
	case ERROR_CODE_LABEL_DOES_NOT_EXISTS: return "Could not find Label in symbol map.";
	case ERROR_CODE_LABEL_ALREADY_EXISTS_AS_EXTERN: return "Label already defined as extern.";
	case ERROR_CODE_LABEL_ALREADY_EXISTS_AS_ENTRY: return "Label already defined as entry.";
	case ERROR_CODE_LABEL_CANNOT_BE_DEFINED_AS_OPCODE_OR_REGISTER: return "Label cannot be defined as Opcode or Register";
	case ERROR_CODE_LABEL_MISSING_OR_NON_EXISTS_OPCODE: return "OPCODE does not exists or missing";
	default: return "Unknown error";
	}
}

