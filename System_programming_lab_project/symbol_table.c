/** @file
*/

#include "symbol_table.h"
#include <string.h>

/**
* Create a new symbol table. The table is empty after this function returns. This can be used to create an uninitialized table without allocating it for the first time.
* 
* 
* @return A pointer to the newly allocated symbol table. NULL on memory exhaustion or if there is insufficient memory
*/
SymbolTable* symbol_table_new_table()
{
    SymbolTable* new_table = (SymbolTable*)xmalloc(sizeof(SymbolTable));
    new_table->head = new_table->tail = NULL;
    new_table->completed = FALSE;
    new_table->hasEntries = FALSE;
    new_table->hasExternals = FALSE;
    return new_table;
}

/**
* Create a new node in the symbol table. It is assumed that the name is unique and that the symbol table is not modified by the caller
* 
* @param name
* @param type - the type of the symbol ( symbolType. c... )
* @param counter - the counter of the symbol ( 0 for global )
* 
* @return pointer to the new node or NULL if there was an error allocating memory ( in which case a message is printed
*/
SymbolTableNode* symbol_table_new_node(char* name, symbolType type, long counter)
{
    SymbolTableNode* node = (SymbolTableNode*)xmalloc(sizeof(SymbolTableNode));
    node->next = NULL;
    node->sym.name = get_copy_string(name);
    node->sym.counter = counter;
    node->sym.type = type;

    return node;
}

/**
* Search a symbol table. This is used to find a symbol by name. If the symbol is found it is returned otherwise NULL is returned.
* 
* @param table
* @param name
* 
* @return pointer to the symbol table node that matches the name or NULL if not found. Note that the caller must free the node
*/
SymbolTableNode* symbol_table_search_symbol(SymbolTable* table, char* name)
{
    LIST_FOR_EACH(SymbolTableNode, table->head, head) {
        if (strcmp(head->sym.name, name) == 0) {
            return head;
        }
    }
    return NULL;
}

/**
* Search a symbol table for a symbol with a given name. This is used to determine if a symbol is defined in the symbol table but does not have to be loaded into memory.
* 
* @param table
* @param name
* 
* @return TRUE if the symbol is defined in the symbol table FALSE otherwise. Note that the return value is undefined if the symbol is not defined
*/
bool symbol_table_search_symbol_bool(SymbolTable* table, char* name)
{
    LIST_FOR_EACH(SymbolTableNode, table->head, head) {
        if (strcmp(head->sym.name, name) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

/**
* Insert a symbol into the table. This is used to insert an entry into a symbol table. The table must be sorted by insertion time and sorted by number of entries.
* 
* @param table
* @param symbol
*/
void symbol_table_insert_symbol(SymbolTable* table, SymbolTableNode* symbol)
{
    if (symbol_table_is_empty(table)) {
        table->tail = table->head = symbol;
    }
    else {
        table->tail->next = symbol;
        table->tail = table->tail->next;
    }
    if (!table->hasEntries) symbol->sym.type == SYM_ENTRY ? table->hasEntries = TRUE : NULL;
    if (!table->hasExternals) symbol->sym.type == SYM_EXTERN ? table->hasExternals = TRUE : NULL;
}

/**
* Updates the amount of items in the symbol table. This is done by taking the number of items in the symbol table and counting them.
* 
* @param table
* 
* @return 1 if the table is larger than the amount of items 0 if it is equal to the amount of
*/
int update_amount_of_items(SymbolTable* table) {
    int count = 0;
    SymbolTableNode* node = table->head;
    while (node != NULL) {
        count++;
        node = node->next;
    }
    table->amountOfSymbols = count;
    return table->amountOfSymbols;
}

/**
* Check if a symbol table is empty. This is used to detect circular references in the symbol table.
* 
* @param table
* 
* @return true if the symbol table is empty false otherwise. Note that the caller must have obtained the lock before calling this
*/
bool symbol_table_is_empty(SymbolTable* table)
{
    return !table->head && !table->tail;
}

/**
* Destroy a symbol table. This frees all memory associated with the symbol table and the table itself.
* 
* @param table
*/
void symbol_table_destroy(SymbolTable** table)
{
    SymbolTableNode* next;
    SymbolTableNode* head = (*table)->head;

   while (head) {
        next = head->next;
        free(head->sym.name);
        free(head);
        head = next;
    }

    free(*table);
}

bool check_symbol_existence(SymbolTable* sym_table, char* name, symbolType newSymType) {
    SymbolTableNode* sym = symbol_table_search_symbol(sym_table, name);

    switch (newSymType)
    {
    case 2: /*entry*/
        if (sym->sym.type == SYM_EXTERN) return TRUE;
        return FALSE;
    case 3: /*extern*/
        if (sym->sym.type == SYM_ENTRY) return TRUE;
        return FALSE;
    default:
        return FALSE;
    }
}