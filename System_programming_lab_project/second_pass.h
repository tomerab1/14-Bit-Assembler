#ifndef SECOND_PASS_H
#define SECOND_PASS_h

#include "line_iterator.h"
#include "symbol_table.h"
#include "constants.h"
#include "memory.h"
#include "debug.h"
#include "utils.h"

typedef struct lines_list_node
{
	int address;
	char* data; /* 14 bits string strings. */
	char* machine_data; /* 14 bits string strings. */
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

bool initiate_second_pass(char* path, SymbolTable* table, int* DC, int* L);

bool generate_object_file(memoryBuffer* memory, char* path, errorContext* err);
LinesListNode translate_to_machine_data(memoryBuffer* memory, errorContext err);

bool generate_externals_file(SymbolTable* table, char* path);
bool generate_entries_file(SymbolTable* table, char* path);

bool order_exists(LineIterator* line, flags* flag);
bool extract_order_type(LineIterator* line, flags* flag);
void skip_label(LineIterator* line);


void* handle_dot_data();
void* handle_dot_string();
void* handle_dot_extern();
void* handle_dot_entry();

bool handle_errors(errorContext* error);

void convert_to_binary(char* data);
void convert_to_deciaml(char* data);
#endif
