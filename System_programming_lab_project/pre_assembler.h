#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "line_iterator.h"
#include "constants.h"
#include "utils.h"

typedef enum { READ_UNKNOWN, READ_COMMENT, READ_START_MACRO, READ_END_MACRO } ReadState;

/* 
	Struct to represent a list node. 
	'macro_expension' - Holds the source code of the macro.
*/
typedef struct __list_node
{
	int log_sz;
	int phy_sz;
	char* macro_name;
	char** macro_expension; // A dynamic array of strings.
	struct __list_node* next;
} MacroListNode;

/*
	Struct to represent a linked list.
*/
typedef struct
{
	MacroListNode* head;
	MacroListNode* tail;
} MacroList;

void do_pre_assembler(const char* path);

ReadState get_current_reading_state(LineIterator* it);

/* Creates a new macro list */
MacroList* get_new_macro_list();

/* Checks if macro list is empty */
bool is_macro_list_empty(MacroList* list);

/* Create a new macro list node */
MacroListNode* create_new_macro_list_node(const char* name);

/* Inserts a new node to the macro list */
void insert_node_to_macro_list(MacroList* list, MacroListNode* node);

/* Inserts a line into '__list_node.macro_expension' member. */
void insert_data_to_macro_list_node(MacroListNode* node, const char* line);

/* Frees the 'macro_expension' member */
void free_macro_expension(char*** macro_expension, int size);

/* Frees a macro list */
void free_macro_list(MacroList** list);

#endif