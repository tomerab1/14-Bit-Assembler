#ifndef SYMBOAL_TABLE_H
#define SYMBOAL_TABLE_H

#include "memory.h"

typedef struct
{
	char* name;
	MemoryWord mem_word;
} Symbol;

typedef struct __sym_table_node
{
	Symbol sym;
	struct __sym_table_node* next;
} SymbolTableNode;

typedef struct
{
	SymbolTableNode* head;
	SymbolTableNode* tail;
} SymbolTable;


#endif
