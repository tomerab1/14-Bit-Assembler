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

debugNode* debug_list_new_node(char* start, char* err, int line, errorCodes err_code)
{
	debugNode* new_node = (debugNode*)xmalloc(sizeof(debugNode));
	/* The node does not own the start and err pointers, we should not call 'free' on them ! */
	new_node->ctx.start_pos = start;
	new_node->ctx.err_pos = err;
	new_node->ctx.line_num = line;
	new_node->ctx.err_code = err_code;
	new_node->next = NULL;
	return new_node;
}

void debug_list_destroy_node(debugNode* node)
{
	node->ctx.start_pos = NULL;
	node->ctx.err_pos = NULL;
}

void debug_list_destroy(debugList** list)
{
	debugNode* head = (*list)->head, *next;

	while (head) {
		next = head->next;
		debug_list_destroy_node(head);
		free(head);
		head = next;
	}

	free(*list);
}

void debug_list_pretty_print(debugList* list)
{
	debugNode* head = list->head;
	char err_buff[80] = { 0 };
	ptrdiff_t offset = 0;	
	size_t err_len = 0;

	while (head) {
		/* Calculate the spacing between the start of the line and the error pos. */
		err_len = sprintf(err_buff, "Line %d:", head->ctx.line_num) + 1;
		offset = err_len + ((head->ctx.err_pos) - (head->ctx.start_pos));

		switch (head->ctx.err_code) {
		default:
			printf("%s %s\n", err_buff, head->ctx.start_pos);
			while (offset > 0) {
				printf(" ");
				offset--;
			}
			printf("^\t\n%s: Invalid token !\n", debug_map_token_to_err(head->ctx.err_code));
			break;
		}

		head = head->next;
	}
}

const char* debug_map_token_to_err(errorCodes code)
{
	switch (code) {

	default:
		return "UnknowError";
	}
}