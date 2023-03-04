#ifndef SYMBOAL_TABLE_H
#define SYMBOAL_TABLE_H

/** @file
*/

#include "utils.h"

typedef enum symbolType { SYM_DATA, SYM_CODE, SYM_ENTRY, SYM_EXTERN } symbolType;

typedef struct Symbol Symbol;

typedef struct SymbolTableNode SymbolTableNode;

typedef struct SymbolTable SymbolTable;

SymbolTable* symbol_table_new_table();

SymbolTableNode* symbol_table_new_node(char* name, symbolType type, long counter);

SymbolTableNode* symbol_table_search_symbol(SymbolTable* table, char* name);

bool symbol_table_search_symbol_bool(SymbolTable* table, char* name);

void symbol_table_insert_symbol(SymbolTable* table, SymbolTableNode* symbol);

bool symbol_table_is_empty(SymbolTable* table);

void symbol_table_destroy(SymbolTable** table);

int update_amount_of_items(SymbolTable* table);

bool check_symbol_existence(SymbolTable* sym_table, char* name, symbolType newSymType);

char* symbol_get_name(Symbol sym);

int symbol_get_counter(Symbol sym);

symbolType symbol_get_type(Symbol sym);

Symbol symbol_node_get_sym(SymbolTableNode* node);

SymbolTableNode* symbol_node_get_next(SymbolTableNode* node);

SymbolTableNode* symbol_table_get_head(SymbolTable* table);

SymbolTableNode* symbol_table_get_tail(SymbolTable* table);

int symbol_table_get_numOfSym(SymbolTable* table);

void symbol_table_set_numOfSym(SymbolTable* table, int num);

bool symbol_table_get_hasExternals(SymbolTable* table);

void symbol_table_set_hasExternals(SymbolTable* table, bool hasExternals);

bool symbol_table_get_hasEntries(SymbolTable* table);

void symbol_table_set_hasEntries(SymbolTable* table, bool hasEntries);

bool symbol_table_get_completed(SymbolTable* table);

void symbol_table_set_completed(SymbolTable* table, bool isCompleted);

#endif
