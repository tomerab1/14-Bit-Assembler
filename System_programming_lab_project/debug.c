#include "debug.h"

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
	size_t len = strlen("Error: ");
	ptrdiff_t offset = 0;

	while (head) {
		/* Calculate the spacing between the start of the line and the error pos. */
		offset = len + ((head->ctx.err_pos) - (head->ctx.start_pos));

		switch (head->ctx.err_code) {
		default:
			printf("Error: %s\n", head->ctx.start_pos);
			while (offset > 0) {
				printf(" ");
				offset--;
			}
			printf("^\n");
			break;
		}

		head = head->next;
	}
}
