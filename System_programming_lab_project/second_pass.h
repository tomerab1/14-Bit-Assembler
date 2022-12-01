#ifndef SECOND_PASS_H
#define SECOND_PASS_h

#include "line_iterator.h"
#include "symbol_table.h"
#include "constants.h"
#include "memory.h"
#include "debug.h"
#include "utils.h"
#include "debug.h"

typedef struct lines_list_node
{
	int address;
	char* data; /* 14 bits string strings. */
	char* machine_data[SINGLE_ORDER_SIZE]; /* 14 bits string strings. */
	char* line_type; /*empty, comment, guidence, command*/
	struct lines_list_node* next;
} LinesListNode;

/*
	Struct to represent a linked list.
*/
typedef struct
{
	LinesListNode* head;
	LinesListNode* tail;
} LinesList;

typedef struct item
{
	int index;
	int type;
} item;

typedef struct flags
{
	bool dot_entry;
	bool dot_extern;
} flags;

typedef struct programFinalStatus
{
	bool createdObject;
	bool createdExternals;
	bool createdEntry;
	errorContext error;
} programFinalStatus;



typedef enum
{
	DOT_DATA_CODE = 1,
	DOT_STRING_CODE,
	DOT_ENTRY_CODE,
	DOT_EXTERN_CODE
};

//starts second pass process
bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory);

//generates object file 
bool generate_object_file(memoryBuffer* memory, char* path, errorContext* err);
//translates data from memory to object text style configuration
LinesList* translate_to_machine_data(memoryBuffer* memory, errorContext* err);
//generates external file
bool generate_externals_file(SymbolTable* table, char* path);
//generates entries file
bool generate_entries_file(SymbolTable* table, char* path);
//checks if any order type (extern or entry) commands exists in the program
bool order_exists(LineIterator* line, flags* flag);
//checks if extern commands exists in the program
bool extern_exists(flags* flag);
//checks if entry commands exists in the program
bool entry_exists(flags* flag);

bool extract_order_type(LineIterator* line, flags* flag);


void* handle_dot_data();
void* handle_dot_string();
void* handle_dot_extern();
void* handle_dot_entry();

//checks if first world is label
bool isLabel(LineIterator* line);
//skip label if exists
void skip_label(LineIterator* line, bool* labelFlag, SymbolTable* table, errorContext* err);

bool handle_errors(errorContext* error);

void convert_to_binary(char* data);
void convert_to_deciaml(char* data);
#endif
