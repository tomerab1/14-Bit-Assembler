#include "pre_assembler.h"
#include <string.h>
#include <ctype.h>

MacroList* create_macro_list_from_file(FILE* in)
{
	LineIterator it;
	ReadState current_state = READ_UNKNOWN;
	MacroList* list = get_new_macro_list();
	bool did_started_reading = FALSE;
	char* line;

	while ((line = get_line(in)) != NULL) {
		line_iterator_put_line(&it, line);

		/* If blanks are encountered, consume them. */
		while (isblank(line_iterator_peek(&it)))
			line_iterator_advance(&it);

		current_state = get_current_reading_state(&it);
		if (current_state != READ_COMMENT) {
			if (current_state == READ_START_MACRO && !did_started_reading) {
				insert_node_to_macro_list(list, create_new_macro_list_node(get_macro_name(&it)));
				did_started_reading = TRUE;
			}
			else if (current_state == READ_END_MACRO) {
				did_started_reading = FALSE;
			}
			else if (did_started_reading) {
				insert_data_to_macro_list_node(list->tail, line);
			}
		}

		free(line);
	}

	return list;
}

void start_pre_assembler(const char* path)
{
	FILE* in = open_file(path, "r");
	MacroList* list = create_macro_list_from_file(in);

	print_macro_list(list);

	/* Moves the file pointer back to the starting of the file. */
	rewind(in);

	/* TODO: Write the program, after running the pre-assembler, to a new file. */

}

ReadState get_current_reading_state(LineIterator* it)
{
	int i;
	ReadState state = READ_UNKNOWN;
	char ch = line_iterator_peek(it);

	if (ch == START_COMMENT_CHAR) {
		return READ_COMMENT;
	}

	line_iterator_advance(it);

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

char* get_macro_name(LineIterator* it)
{
	int phy_sz = INIT_PHY_SZ, log_sz = INIT_LOG_SZ;
	char* name = (char*)calloc(INIT_PHY_SZ, sizeof(char));

	while (line_iterator_peek(it) != '\n') {
		if (log_sz + 1 >= phy_sz) {
			GROW_CAPACITY(phy_sz);
			name = GROW_ARRAY(char*, name, phy_sz, sizeof(char));
		}
		name[log_sz++] = line_iterator_peek(it);
		line_iterator_advance(it);
	}

	if (log_sz < phy_sz) {
		name = GROW_ARRAY(char*, name, log_sz + 1, sizeof(char));
	}

	name[log_sz] = '\0';
	return name;
}

MacroList* get_new_macro_list()
{
	MacroList* new_list = (MacroList*) xmalloc(sizeof(MacroList));
	new_list->head = new_list->tail = NULL;
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

void print_macro_list(MacroList* list)
{
	MacroListNode* head = list->head;
	int i;

	while (head) {
		printf("MACRO: %s\n", head->macro_name);

		for (i = 0; i < head->log_sz; i++) {
			printf("%s", head->macro_expension[i]);
		}

		printf("\n");
		head = head->next;
	}
}

void free_macro_list(MacroList** list)
{
	MacroListNode* next, *current = (*list)->head;

	while (current) {
		next = current->next;
		free(current->macro_name);
		free_macro_expension(&current->macro_expension, current->log_sz);
		free(current);
		current = next;
	}
	free(*list);
}
