#ifndef SYMBOAL_TABLE_H
#define SYMBOAL_TABLE_H

/** @file
*/

#include "utils.h"

typedef enum { SYM_DATA, SYM_CODE, SYM_ENTRY, SYM_EXTERN } symbolType;

typedef struct
{
	char* name;
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
	int amountOfSymbols;
	bool hasExternals;
	bool hasEntries;
	bool completed;
} SymbolTable;

/**
* @brief Create a new symbol table. The table is empty after this function returns. This can be used to create an uninitialized table without allocating it for the first time.
* @return A pointer to the newly allocated symbol table. NULL on memory exhaustion or if there is insufficient memory
*/
SymbolTable* symbol_table_new_table();

/**
* @brief Create a new node in the symbol table. It is assumed that the name is unique and that the symbol table is not modified by the caller
* @param name
* @param type - the type of the symbol ( symbolType. c... )
* @param counter - the counter of the symbol ( 0 for global )
* @return pointer to the new node or NULL if there was an error allocating memory ( in which case a message is printed
*/
SymbolTableNode* symbol_table_new_node(char* name, symbolType type, long counter);

/**
* @brief Search a symbol table. This is used to find a symbol by name. If the symbol is found it is returned otherwise NULL is returned.
* @param table
* @param name
* @return pointer to the symbol table node that matches the name or NULL if not found. Note that the caller must free the node
*/
SymbolTableNode* symbol_table_search_symbol(SymbolTable* table, char* name);

/**
* @brief Search a symbol table for a symbol with a given name. This is used to determine if a symbol is defined in the symbol table but does not have to be loaded into memory
* @param table
* @param name
* @return TRUE if the symbol is defined in the symbol table FALSE otherwise. Note that the return value is undefined if the symbol is not defined
*/
bool symbol_table_search_symbol_bool(SymbolTable* table, char* name);

/**
* @brief Insert a symbol into the table. This is used to insert an entry into a symbol table. The table must be sorted by insertion time and sorted by number of entries.
* @param table
* @param symbol
*/
void symbol_table_insert_symbol(SymbolTable* table, SymbolTableNode* symbol);

/**
* @brief Check if a symbol table is empty. This is used to detect circular references in the symbol table.
* @param table
* @return true if the symbol table is empty false otherwise. Note that the caller must have obtained the lock before calling this
*/
bool symbol_table_is_empty(SymbolTable* table);

/**
* @brief Destroy a symbol table. This frees all memory associated with the symbol table and the table itself.
* @param table
*/
void symbol_table_destroy(SymbolTable** table);

/**
* @brief Updates the amount of items in the symbol table. This is done by taking the number of items in the symbol table and counting them.
* @param table
* @return 1 if the table is larger than the amount of items 0 if it is equal to the amount of
*/
int update_amount_of_items(SymbolTable* table);

/**

@brief Check if a symbol with the given name exists in the symbol table and has a specific type.
@param sym_table A pointer to the symbol table to search in.
@param name A string containing the name of the symbol to search for.
@param newSymType An enumeration value representing the type of the symbol to check for (either SYM_ENTRY or SYM_EXTERN).
@return A boolean value indicating whether a symbol with the given name and type was found in the symbol table or not.
This function searches the given symbol table for a symbol with the given name and checks if it has the specified type (either SYM_ENTRY or SYM_EXTERN). If the symbol is found and has the correct type, the function returns TRUE. Otherwise, it returns FALSE.
Note that if the symbol is not found in the symbol table, the function will also return FALSE.
*/
bool check_symbol_existence(SymbolTable* sym_table, char* name, symbolType newSymType);

#endif
