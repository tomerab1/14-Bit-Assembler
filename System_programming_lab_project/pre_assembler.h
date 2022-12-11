#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

/** @file
*/

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
	char** macro_expension; /* A dynamic array of strings. */
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

/* Starts the pre-assembler phase. */
void start_pre_assembler(char* path);

/* Reads a file, fills 'in_list' with the macros data, if all is valid, it returns TRUE, otherwise FALSE. */
void macro_list_fill_list_from_file(FILE* in, MacroList* in_list);

/* Returns the current reading state. */
ReadState get_current_reading_state(LineIterator* it);

/* Returns the macro label name. */
char* get_macro_name(LineIterator* it);

/* Creates a new macro list */
MacroList* macro_list_new_list();

/* Checks if macro list is empty */
bool macro_list_is_empty(MacroList* list);

/* Create a new macro list node */
MacroListNode* macro_list_new_node(char* name);

/* Expands a macro to file 'out'. */
void expand_macro_to_file(FILE* out, MacroList* list, char* name);

/* Inserts a new node to the macro list */
void macro_list_insert_node(MacroList* list, MacroListNode* node);

/* Inserts a line into '__list_node.macro_expension' member. */
void macro_list_node_insert_source(MacroListNode* node, char* line);

/* Insert all of the data inside the macro specified in 'line' to the current node */
void macro_list_node_insert_macro(MacroListNode* tail, MacroListNode* node);

/* Returns the node in which its name matches to entry*/
MacroListNode* macro_list_get_node(MacroList* list, char* entry);

/* Creates an expanded source file inside 'out' */
void create_pre_assembler_file(FILE* in, FILE* out, MacroList* list);

/* Frees the 'macro_expension' member */
void macro_free_expension(char*** macro_expension, int size);

#ifdef DEBUG
void macro_list_dump(MacroList* list);
#endif 

/* Frees a macro list */
void macro_list_free(MacroList** list);

#endif