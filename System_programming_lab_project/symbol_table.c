/** @file
*/

#include "symbol_table.h"
#include <string.h>

typedef struct Symbol
{
    char* name;
    int counter;
    symbolType type;
} Symbol;

typedef struct SymbolTableNode
{
    Symbol sym;
    struct SymbolTableNode* next;
} SymbolTableNode;

typedef struct SymbolTable
{
    SymbolTableNode* head;
    SymbolTableNode* tail;
    int amountOfSymbols;
    bool hasExternals;
    bool hasEntries;
    bool completed;
} SymbolTable;

SymbolTable* symbol_table_new_table()
{
    SymbolTable* new_table = (SymbolTable*)xmalloc(sizeof(SymbolTable));

    new_table->head = new_table->tail = NULL;
    new_table->amountOfSymbols = 0;
    new_table->completed = FALSE;
    new_table->hasEntries = FALSE;
    new_table->hasExternals = FALSE;

    return new_table;
}

SymbolTableNode* symbol_table_new_node(char* name, symbolType type, long counter)
{
    SymbolTableNode* node = (SymbolTableNode*)xmalloc(sizeof(SymbolTableNode));
    node->next = NULL;
    node->sym.name = get_copy_string(name);
    node->sym.counter = counter;
    node->sym.type = type;

    return node;
}

SymbolTableNode* symbol_table_search_symbol(SymbolTable* table, char* name)
{
    LIST_FOR_EACH(SymbolTableNode, table->head, head) {
        if (strcmp(head->sym.name, name) == 0) {
            return head;
        }
    }
    return NULL;
}

bool symbol_table_search_symbol_bool(SymbolTable* table, char* name)
{
    LIST_FOR_EACH(SymbolTableNode, table->head, head) {
        if (strcmp(head->sym.name, name) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

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

bool symbol_table_is_empty(SymbolTable* table)
{
    return !table->head && !table->tail;
}

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

/**

@brief Check if a symbol with the given name exists in the symbol table and has a specific type.
@param sym_table A pointer to the symbol table to search in.
@param name A string containing the name of the symbol to search for.
@param newSymType An enumeration value representing the type of the symbol to check for (either SYM_ENTRY or SYM_EXTERN).
@return A boolean value indicating whether a symbol with the given name and type was found in the symbol table or not.
This function searches the given symbol table for a symbol with the given name and checks if it has the specified type (either SYM_ENTRY or SYM_EXTERN). If the symbol is found and has the correct type, the function returns TRUE. Otherwise, it returns FALSE.
Note that if the symbol is not found in the symbol table, the function will also return FALSE.
*/
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

char* symbol_get_name(Symbol* sym)
{
    return sym->name;
}

int symbol_get_counter(Symbol* sym)
{
    return sym->counter;
}

void symbol_set_counter(Symbol* sym, int num)
{
    sym->counter = num;
}

symbolType symbol_get_type(Symbol* sym)
{
    return sym->type;
}

Symbol* symbol_node_get_sym(SymbolTableNode* node)
{
    return &node->sym;
}

SymbolTableNode* symbol_node_get_next(SymbolTableNode* node)
{
    return node->next;
}

SymbolTableNode* symbol_table_get_head(SymbolTable* table)
{
    return table->head;
}

SymbolTableNode* symbol_table_get_tail(SymbolTable* table)
{
    return table->tail;
}

int symbol_table_get_numOfSym(SymbolTable* table)
{
    return table->amountOfSymbols;
}

void symbol_table_set_numOfSym(SymbolTable* table, int num)
{
    table->amountOfSymbols = num;
}

bool symbol_table_get_hasExternals(SymbolTable* table)
{
    return table->hasExternals;
}

void symbol_table_set_hasExternals(SymbolTable* table, bool hasExternals)
{
    table->hasExternals = hasExternals;
}

bool symbol_table_get_hasEntries(SymbolTable* table)
{
    return table->hasEntries;
}

void symbol_table_set_hasEntries(SymbolTable* table, bool hasEntries)
{
    table->hasEntries = hasEntries;
}

bool symbol_table_get_completed(SymbolTable* table)
{
    return table->completed;
}

void symbol_table_set_completed(SymbolTable* table, bool isCompleted)
{
    table->completed = isCompleted;
}
