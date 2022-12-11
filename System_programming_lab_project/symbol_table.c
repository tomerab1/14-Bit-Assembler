/** @file
*/

#include "symbol_table.h"
#include "utils.h"
#include <string.h>

SymbolTable* symbol_table_new_table()
{
    SymbolTable* new_table = (SymbolTable*)xmalloc(sizeof(SymbolTable));
    new_table->head = new_table->tail = NULL;
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
}

void update_amount_of_items(SymbolTable* table) {
    int count = 0;
    SymbolTableNode* node = table->head;
    while (node != NULL) {
        count++;
        node = node->next;
    }
    table->amountOfSymbols = count;
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
