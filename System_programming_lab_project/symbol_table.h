#ifndef SYMBOAL_TABLE_H
#define SYMBOAL_TABLE_H

#include "utils.h"

typedef enum { SYM_DATA, SYM_CODE, SYM_ENTRY, SYM_EXTERN } symbolType;

typedef struct
{
	const char* name;
	long counter;
	symbolType type;

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

SymbolTable* symbol_table_new_table();
SymbolTableNode* symbol_table_new_node(const char* name, symbolType type, long counter);
SymbolTableNode* symbol_table_search_symbol(SymbolTable* table, const char* name);
void symbol_table_insert_symbol(SymbolTable* table, SymbolTableNode* symbol);
bool symbol_table_is_empty(SymbolTable* table);
void symbol_table_destroy(SymbolTable** table);

#endif
