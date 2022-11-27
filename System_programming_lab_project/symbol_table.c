#include "symbol_table.h"
#include <string.h>

SymbolTable* symbol_table_new_table()
{
    SymbolTable* new_table = (SymbolTable*)xmalloc(sizeof(SymbolTable));
    new_table->head = new_table->tail = NULL;
    return new_table;
}

SymbolTableNode* symbol_table_new_node(const char* name, MemoryWord word)
{
    SymbolTableNode* node = (SymbolTableNode*)xmalloc(sizeof(SymbolTableNode));
    node->next = NULL;
    node->sym.name = name; /* Don't free me :) */
    node->sym.mem_word = word;
    return node;
}

SymbolTableNode* symbol_table_search_symbol(SymbolTable* table, const char* name)
{
    LIST_FOR_EACH(SymbolTableNode, table->head, head) {
        if (strcmp(head->sym.name, name) == 0) {
            return head;
        }
    }
    return NULL;
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

bool symbol_table_is_empty(SymbolTable* table)
{
    return !table->head && !table->tail;
}

void symbol_table_destroy(SymbolTable** table)
{
    SymbolTableNode* next;
    LIST_FOR_EACH(SymbolTableNode, (*table)->head, head) {
        next = head->next;
        free(head);
        head = next;
    }
    free(table);
}
