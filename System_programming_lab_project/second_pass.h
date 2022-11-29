#ifndef SECOND_PASS_H
#define SECOND_PASS_h

#include "line_iterator.h"
#include "symbol_table.h"
#include "constants.h"
#include "utils.h"
#include "debug.h"

typedef struct lines_list_node
{
	int log_sz;
	int phy_sz;
	int address;
	char* data; /* 14 bits string strings. */
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

typedef enum
{
	DOT_DATA = 1,
	DOT_STRING,
	DOT_ENTRY,
	DOT_EXTERN
};

bool initiate_second_pass(char* path);

bool generate_object_file(FILE* out, char* data, int orders_length, int data_length);
bool generate_externals_file(FILE* out, void* data, bool isExists);
bool generate_entries_file(FILE* out, void* data, bool isExists);

int extract_order_type(char* line);
void* handle_dot_data();
void* handle_dot_string();
void* handle_dot_extern();
void* handle_dot_entry();

bool entry_exists();
bool extern_exists();

void handle_errors(errorCodes err);
int find_symbol_in_table(char* symbol);

void convert_to_binary(char* data);
void convert_to_deciaml(char* data);
#endif
