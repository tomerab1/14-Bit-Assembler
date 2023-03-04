#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

/** @file
*/

#include "line_iterator.h"
#include "constants.h"
#include "utils.h"

/**
* @brief Enum for the constans for the different reading states, it only used internally so it'll be declared inside the '.c' file.
*/
typedef enum { READ_UNKNOWN, READ_COMMENT, READ_START_MACRO, READ_END_MACRO } ReadState;

/**
* @brief A forward declaration of the macro list node which hold the content of each macro, will be declared in the '.c' file.
*/
typedef struct macro_list_node MacroListNode;

/**
* @brief A forward declaration of the macro list node which stores all the macros in the file, will be declared in the '.c' file.
*/
typedef struct macro_list MacroList;

/**
* @brief This function starts the pre-assembler phase of expanding the macros.
* @param path - The path of the source file.
*/
void start_pre_assembler(char* path);

/* Reads a file, fills 'in_list' with the macros data, if all is valid, it returns TRUE, otherwise FALSE. */
/**
* @brief This function creates the macro list from a given file.
* @param in - The input file.
* @param in_list - The list.
*/
void macro_list_fill_list_from_file(FILE* in, MacroList* in_list);

/**
* @brief This function returns the current reading state.
* @param it - The line iterator.
*/
ReadState get_current_reading_state(LineIterator* it);

/**
* @brief This function return the name of a macro.
* @param it - The line iterator.
* @return The macros name if there is one, false otherwise.
*/
char* get_macro_name(LineIterator* it);

/**
* @brief This function creates a new empty macro list.
* @return An empty macro list.
*/
MacroList* macro_list_new_list();

/**
* @brief This function check if the list passed to it is empty.
* @param list - The list.
* @return True if empty, False otherwise.
*/
bool macro_list_is_empty(MacroList* list);

/**
* @brief This function create a new MacroListNode.
* @param name - The macros name.
* @return A new MacroListNode.
*/
MacroListNode* macro_list_new_node(char* name);

/**
* @brief This function expands a macro to the output file.
* @param out - The output file.
* @param list - The macros list.
* @param name - The macros name.
*/
void expand_macro_to_file(FILE* out, MacroList* list, char* name);

/**
* @brief This function insert a node to the macro list.
* @param list - The list.
* @param node - The node to insert.
*/
void macro_list_insert_node(MacroList* list, MacroListNode* node);

/**
* @brief This function inserts a line into 'macro_list_node.macro_expension' member.
* @param node - The node.
* @param line - The line.
*/
void macro_list_node_insert_source(MacroListNode* node, char* line);

/**
* @brief This function insert all of the data inside the macro specified in 'line' to the current node.
* @param tail - The lists tail.
* @param node - The node.
*/
void macro_list_node_insert_macro(MacroListNode* tail, MacroListNode* node);

/**
* @brief Returns the node in which its name matches to entry
* @param list - The list.
* @param entry - The entry.
* @return The MacroListNode that matches the entry, NULL otherwise.
*/
MacroListNode* macro_list_get_node(MacroList* list, char* entry);

/**
* @brief Creates an expanded source file inside 'out'.
* @param in - The input file.
* @param out - The output file.
* @param list - The macros list.
*/
void create_pre_assembler_file(FILE* in, FILE* out, MacroList* list);

/**
* @brief This function frees the 'macro_expension' member
* @param macro_expension - The macro to free.
* @param size - The number of lines of the macro.
*/
void macro_free_expension(char*** macro_expension, int size);

/**
* @brief This function frees a macro list.
* @param list - The list to free.
*/
void macro_list_free(MacroList** list);

#endif