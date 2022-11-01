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
bool start_pre_assembler(const char* path);

/* Reads a file, fills 'in_list' with the macros data, if all is valid, it returns TRUE, otherwise FALSE. */
bool fill_macro_list_from_file(FILE* in, MacroList* in_list);

/* Returns the current reading state. */
ReadState get_current_reading_state(LineIterator* it);

/* Searches the list for an enrty. */
bool search_macro_list(const MacroList* list, const char* entry);

/* Checks if the name is not a keyword, and it's not a label. */
bool is_a_macro_name(const char* name);

/* Returns the macro label name. */
char* get_macro_name(LineIterator* it);

/* Creates a new macro list */
MacroList* get_new_macro_list();

/* Checks if macro list is empty */
bool is_macro_list_empty(MacroList* list);

/* Create a new macro list node */
MacroListNode* create_new_macro_list_node(const char* name);

/* Expands a macro to file 'out'. */
void expand_macro_to_file(FILE* out, MacroList* list, const char* name);

/* Inserts a new node to the macro list */
void insert_node_to_macro_list(MacroList* list, MacroListNode* node);

/* Inserts a line into '__list_node.macro_expension' member. */
void insert_data_to_macro_list_node(MacroListNode* node, const char* line);

/* Insert all of the data inside the macro specified in 'line' to the current node */
void insert_macro_data_to_list_node(MacroListNode* tail, MacroListNode* node);

/* Returns the node in which its name matches to entry*/
MacroListNode* get_macro_list_node(MacroList* list, const char* entry);

/* Creates an expanded source file inside 'out' */
void create_pre_assembler_file(FILE* in, FILE* out, MacroList* list);

/* Frees the 'macro_expension' member */
void free_macro_expension(char*** macro_expension, int size);

#ifdef DEBUG
void dump_macro_list(MacroList* list);
#endif 


/* Frees a macro list */
void free_macro_list(MacroList** list);

#endif