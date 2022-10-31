#include "pre_assembler.h"
#include <string.h>

void do_pre_assembler(const char* path)
{
	LineIterator it;
	ReadState current_state = READ_UNKNOWN;
	MacroList list = get_new_macro_list();
	MacroListNode* node;
	bool did_started_reading = FALSE;
	char* line;
	FILE* f_in = open_file(path, "r");

	while ((line = get_line(f_in)) != NULL) {
		line_iterator_put_line(&it, line);
		while (!line_iterator_is_end(&it)) {
			if (current_state != READ_COMMENT) {
				current_state = get_current_reading_state(&it, line_iterator_peek(&it));
				if (current_state == READ_START_MACRO && !did_started_reading) {
					insert_node_to_macro_list(&list, get_macro_name(&it));
					did_started_reading = TRUE;
				}

				if (current_state == READ_END_MACRO) {
					did_started_reading = FALSE;
				}

				else if (did_started_reading && current_state == READ_UNKNOWN) {
					insert_data_to_macro_list_node(list.tail, line);
				}
			}

			line_iterator_advance(&it);
		}

		free(line);
	}

	fclose(f_in);
	free_macro_list(&list);
}

ReadState get_current_reading_state(LineIterator* it, char ch)
{
	int i;
	ReadState state = READ_UNKNOWN;

	if (ch == START_COMMENT_CHAR) {
		return READ_COMMENT;
	}

	if (ch == START_MACRO_DEFENITION[0]) { /* Matches first char, 'm' */
		for (i = 1; i < START_MACRO_DEF_LEN && !line_iterator_is_end(it); i++, line_iterator_advance(it)) {
			if (line_iterator_peek(it) != START_MACRO_DEFENITION[i])
				return READ_UNKNOWN;
		}

		return READ_START_MACRO;
	}

	if (ch == END_MACRO_DEFENITION[0]) { /* Matches first char, 'e' */
		for (i = 1; i < END_MACRO_DEF_LEN && !line_iterator_is_end(it); i++, line_iterator_advance(it)) {
			if (line_iterator_peek(it) != END_MACRO_DEFENITION[i])
				return READ_UNKNOWN;
		}

		return READ_END_MACRO;
	}
}

MacroList get_new_macro_list()
{
	MacroList new_list;
	new_list.head = new_list.tail = NULL;
	return new_list;
}

bool is_macro_list_empty(MacroList* list)
{
	return !list->head && !list->tail;
}

MacroListNode* create_new_macro_list_node(const char* name)
{
	MacroListNode* new_node = (MacroListNode*) xmalloc(sizeof(MacroListNode));

	new_node->macro_name = (char*) xcalloc(strlen(name) + 1, sizeof(char));
	sprintf(new_node->macro_name, "%s", name);

	new_node->log_sz = INIT_LOG_SZ;
	new_node->phy_sz = INIT_PHY_SZ;

	new_node->macro_expension = (char**) xcalloc(INIT_PHY_SZ, sizeof(char*));
	new_node->next = NULL;

	return new_node;
}

void insert_node_to_macro_list(MacroList* list, MacroListNode* node)
{
	if (is_macro_list_empty(list)) {
		list->head = list->tail = node;
	}
	else {
		list->tail->next = node;
		list->tail = list->tail->next;
	}
}

void insert_data_to_macro_list_node(MacroListNode* node, const char* line)
{
	size_t text_length = strlen(line);

	if (node->log_sz + 1 >= node->phy_sz) {
		GROW_CAPACITY(node->phy_sz);
		node->macro_expension = GROW_ARRAY(char**, node->macro_expension, node->phy_sz, sizeof(char*));
	}

	node->macro_expension[node->log_sz] = (char*)xcalloc(text_length + 1, sizeof(char));
	sprintf(node->macro_expension[node->log_sz], "%s", line);
	node->log_sz++;
}

void free_macro_expension(char*** macro_expension, int size)
{
	char** ptr = *macro_expension;
	int i;

	for (i = 0; i < size; i++) {
		free(ptr[i]);
	}

	free(ptr);
}

void free_macro_list(MacroList* list)
{
	MacroListNode* next, *current = list->head;

	while (current) {
		next = current->next;
		free(current->macro_name);
		free_macro_expension(&current->macro_expension, current->log_sz);
		free(current);
		current = next;
	}
}
