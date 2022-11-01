#include "pre_assembler.h"
#include <string.h>
#include <ctype.h>

bool fill_macro_list_from_file(FILE* in, MacroList* in_list)
{
	int current_line = 1;
	char* line, *name;
	LineIterator it;
	ReadState current_state = READ_UNKNOWN;
	bool did_started_reading = FALSE, did_error_occurred = FALSE;

	while ((line = get_line(in)) != NULL) {
		name = NULL;
		line_iterator_put_line(&it, line);

		/* If blanks are encountered, consume them. */
		while (isblank(line_iterator_peek(&it)))
			line_iterator_advance(&it);

		/* Get the current state. */
		current_state = get_current_reading_state(&it);

		/* Skip comments, and if not a comment, copy macro (if the state is correct). */
		if (current_state != READ_COMMENT) {
			name = get_macro_name(&it);

			/* Check if a macro was decalred before it was defined. */
			if (current_state != READ_START_MACRO && current_state != READ_END_MACRO && !search_macro_list(in_list, name) && is_a_macro_name(name)) {
				printf("Error on line: %d - Macro defined before declared !\n", current_line);
				did_error_occurred = TRUE;
			}
			if (current_state == READ_START_MACRO && !did_started_reading) {
				/* Verify that a macros name is not a reserved keyword. */
				if (get_opcode(name) != OP_UNKNOWN) {
					printf("Error on line: %d - '%s' is a reserved keyword ! It cannot be used as a macro's name...\n", current_line, name);
					did_error_occurred = TRUE;
				}
				if (!did_error_occurred) {
					insert_node_to_macro_list(in_list, create_new_macro_list_node(name));
				}
				did_started_reading = TRUE;
			}
			else if (current_state == READ_START_MACRO && did_started_reading) {
				printf("Error on line: %d - Nested macro defenition detected !\n", current_line);
				did_error_occurred = TRUE;
			}
			else if (current_state == READ_END_MACRO) {
				did_started_reading = FALSE;
			}
			else if (did_started_reading && !did_error_occurred) {
				/* A macro declaration inside of the macro, we need to insert all of it's source line to those of the current macro. */
				if (search_macro_list(in_list, name)) {
					insert_macro_data_to_list_node(in_list->tail, get_macro_list_node(in_list, name));
				}
				else {
					insert_data_to_macro_list_node(in_list->tail, line);
				}
			}
		}
		current_line++;
		free(name);
		free(line);
	}

	return did_error_occurred;
}

bool start_pre_assembler(const char* path)
{
	FILE* in = open_file(path, MODE_READ), *out = NULL;
	MacroList* list = get_new_macro_list();
	bool did_error_occurred = fill_macro_list_from_file(in, list);
	const char* out_name = NULL;

	/* If an error occurred, we dont create an expanded file, we terminate the assembling process. */
	if (did_error_occurred) {
		/* Cleaning up. */
		free_macro_list(&list);
		fclose(in);
		return FALSE;
	}

#ifdef DEBUG
	dump_macro_list(list);
#endif

	/* Moves the file pointer back to the starting of the file. */
	rewind(in);

	out_name = get_outfile_name(path, ".am");
	out = open_file(out_name, MODE_WRITE);

	create_pre_assembler_file(in, out, list);

	/* Cleaning up. */
	free_macro_list(&list);
	fclose(out);
	fclose(in);

	return TRUE;
}

ReadState get_current_reading_state(LineIterator* it)
{
	int i;
	char ch = line_iterator_peek(it);

	if (ch == START_COMMENT_CHAR) {
		return READ_COMMENT;
	}

	if (ch == START_MACRO_DEFENITION[0]) { /* Matches first char, 'm' */
		line_iterator_advance(it);
		for (i = 1; i < START_MACRO_DEF_LEN && !line_iterator_is_end(it); i++, line_iterator_advance(it)) {
			if (line_iterator_peek(it) != START_MACRO_DEFENITION[i]) {
				line_iterator_backwards(it);
				return READ_UNKNOWN;
			}
		}

		return READ_START_MACRO;
	}

	if (ch == END_MACRO_DEFENITION[0]) { /* Matches first char, 'e' */
		line_iterator_advance(it);
		for (i = 1; i < END_MACRO_DEF_LEN && !line_iterator_is_end(it); i++, line_iterator_advance(it)) {
			if (line_iterator_peek(it) != END_MACRO_DEFENITION[i])
				return READ_UNKNOWN;
		}

		return READ_END_MACRO;
	}
}

bool search_macro_list(const MacroList* list, const char* entry)
{
	MacroListNode* head = list->head;

	while (head) {
		if (strcmp(head->macro_name, entry) == 0) {
			return TRUE;
		}
		head = head->next;
	}

	return FALSE;
}

bool is_a_macro_name(const char* name)
{
	return get_opcode(name) == OP_UNKNOWN && !(strchr(name, ':'));
}

char* get_macro_name(LineIterator* it)
{
	int phy_sz = INIT_PHY_SZ, log_sz = INIT_LOG_SZ;
	char* name = (char*)calloc(INIT_PHY_SZ, sizeof(char));

	while (isblank(line_iterator_peek(it)))
		line_iterator_advance(it);

	while (line_iterator_peek(it) != '\n' && !isblank(line_iterator_peek(it))) {
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

void expand_macro_to_file(FILE* out, MacroList* list, const char* name)
{
	int i;
	MacroListNode* head = list->head;

	while (head) {
		if (strcmp(head->macro_name, name) == 0) {
			for (i = 0; i < head->log_sz; i++) {
				fputs(head->macro_expension[i], out);
			}
		}
		head = head->next;
	}
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

void insert_macro_data_to_list_node(MacroListNode* tail, MacroListNode* node)
{
	int i;
	for (i = 0; i < node->log_sz; i++) {
		insert_data_to_macro_list_node(tail, node->macro_expension[i]);
	}
}

MacroListNode* get_macro_list_node(MacroList* list, const char* entry)
{
	MacroListNode* head = list->head;

	while (head) {
		if (strcmp(head->macro_name, entry) == 0) return head;
		head = head->next;
	}

	return NULL;
}

void create_pre_assembler_file(FILE* in, FILE* out, MacroList* list)
{
	char* line, *name;
	LineIterator it;
	ReadState current_state = READ_UNKNOWN;
	bool did_started_reading = FALSE;

	while ((line = get_line(in)) != NULL) {
		name = NULL;
		line_iterator_put_line(&it, line);

		/* If blanks are encountered, consume them. */
		while (isblank(line_iterator_peek(&it)))
			line_iterator_advance(&it);

		/* Get the current state. */
		current_state = get_current_reading_state(&it);

		/* Skip comments, and if not a comment, copy macro (if the state is correct). */
		if (current_state != READ_COMMENT) {
			name = get_macro_name(&it);

			/* Check if a macros name was encountered */
			if (search_macro_list(list, name)) {
				expand_macro_to_file(out, list, name);
				did_started_reading = TRUE;
			}
			else if (current_state == READ_END_MACRO) {
				did_started_reading = FALSE;
			}
			else {
				/* So the macro wont be copied twice. */
				if (!did_started_reading) {
					fputs(line, out);
				}
			}
		}
		free(name);
		free(line);
	}
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

void dump_macro_list(MacroList* list)
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
