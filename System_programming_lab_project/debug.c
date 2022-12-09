#include "debug.h"
#include <string.h>

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
	char err_buff[80] = { 0 };

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

char* debug_map_token_to_err(errorCodes code)
{
	switch (code) {
	case ERROR_CODE_TO_MANY_OPERANDS: return "To many operands";
	case ERROR_CODE_SYNTAX_ERROR: return "Syntax error";
	case ERROR_CODE_EXTRA_COMMA: return "Extra comma";
    case ERROR_CODE_SYMBOL_REDEFINITION: return "Symbol redefinition";
	default:
		return "UnknownError";
	}
}