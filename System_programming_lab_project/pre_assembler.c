/** @file
*/

#include "pre_assembler.h"
#include <string.h>
#include <ctype.h>


struct macro_list_node
{
    int log_sz;
    int phy_sz;
    char* macro_name;
    char** macro_expension; /* A dynamic array of strings. */
    struct macro_list_node* next;
};

struct macro_list
{
    MacroListNode* head;
    MacroListNode* tail;
};

void macro_list_fill_list_from_file(FILE* in, MacroList* in_list)
{
    char* line, * name;
    LineIterator it;
    MacroListNode* node;
    ReadState current_state = READ_UNKNOWN;
    bool did_started_reading = FALSE;

    while ((line = get_line(in)) != NULL) {
        name = NULL;
        line_iterator_put_line(&it, line);

        if (line_iterator_is_end(&it)) {
            free(line);
            continue;
        }

        /* If blanks are encountered, consume them. */
        line_iterator_consume_blanks(&it);

        /* Get the current state. */
        current_state = get_current_reading_state(&it);

        /* Skip comments, and if not a comment, copy macro (if the state is correct). */
        if (current_state != READ_COMMENT) {
            name = get_macro_name(&it);

            if (current_state == READ_START_MACRO && !did_started_reading) {
                macro_list_insert_node(in_list, macro_list_new_node(name));
                did_started_reading = TRUE;
            }
            else if (current_state == READ_END_MACRO) {
                did_started_reading = FALSE;
            }
            else if (did_started_reading) {
                /* A macro declaration inside of the macro, we need to insert all of it's source line to those of the current macro. */
                node = macro_list_get_node(in_list, name);
                if (node) {
                    macro_list_node_insert_macro(in_list->tail, node);
                }
                else {
                    macro_list_node_insert_source(in_list->tail, line);
                }
            }
        }

        free(name);
        free(line);
    }
}

void start_pre_assembler(char* path)
{
    FILE* in = open_file(path, MODE_READ), * out = NULL;
    MacroList* list = macro_list_new_list();
    char* out_name = NULL;

    macro_list_fill_list_from_file(in, list);

    /* Moves the file pointer back to the starting of the file. */
    rewind(in);

    out_name = get_outfile_name(path, PRE_ASSEMBLER_FILE_EXTENSTION);
    out = open_file(out_name, MODE_WRITE);

    create_pre_assembler_file(in, out, list);

    /* Cleaning up. */
    macro_list_free(&list);
    free(out_name);
    fclose(out);
    fclose(in);
}

ReadState get_current_reading_state(LineIterator* it)
{
    int i;
    char ch = line_iterator_peek(it);

    line_iterator_advance(it);

    switch (ch) {
    case START_COMMENT_CHAR:
        return READ_COMMENT;
    case START_MACRO_DEF_CHAR:
        for (i = 1; i < START_MACRO_DEF_LEN && !line_iterator_is_end(it); i++, line_iterator_advance(it)) {
            if (line_iterator_peek(it) != START_MACRO_DEFENITION[i]) {
                line_iterator_backwards(it);
                return READ_UNKNOWN;
            }
        }
        return READ_START_MACRO;
    case END_MACRO_DEF_CHAR:
        for (i = 1; i < END_MACRO_DEF_LEN && !line_iterator_is_end(it); i++, line_iterator_advance(it)) {
            if (line_iterator_peek(it) != END_MACRO_DEFENITION[i]) {
                line_iterator_backwards(it);
                return READ_UNKNOWN;
            }
        }
        return READ_END_MACRO;
    default:
        return READ_UNKNOWN;
    }
}

char* get_macro_name(LineIterator* it)
{
    int phy_sz = INIT_PHY_SZ, log_sz = INIT_LOG_SZ;
    char* name = (char*)xcalloc(INIT_PHY_SZ, sizeof(char));

    /* If blanks are encountered, consume them. */
    line_iterator_consume_blanks(it);

    while (!line_iterator_is_end(it) && !isblank(line_iterator_peek(it))) {
        if (log_sz + 1 >= phy_sz) {
            GROW_CAPACITY(phy_sz);
            name = GROW_ARRAY(char*, name, phy_sz, sizeof(char));
        }
        name[log_sz++] = line_iterator_peek(it);
        line_iterator_advance(it);
    }

    if (log_sz < phy_sz) {
        name = GROW_ARRAY(char*, name, log_sz + 1, sizeof(char));
    }

    name[log_sz] = '\0';
    return name;
}

MacroList* macro_list_new_list()
{
    MacroList* new_list = (MacroList*)xmalloc(sizeof(MacroList));
    new_list->head = new_list->tail = NULL;
    return new_list;
}

bool macro_list_is_empty(MacroList* list)
{
    return !list->head && !list->tail;
}

MacroListNode* macro_list_new_node(char* name)
{
    MacroListNode* new_node = (MacroListNode*)xmalloc(sizeof(MacroListNode));

    new_node->macro_name = (char*)xcalloc(strlen(name) + 1, sizeof(char));
    sprintf(new_node->macro_name, "%s", name);

    new_node->log_sz = INIT_LOG_SZ;
    new_node->phy_sz = INIT_PHY_SZ;

    new_node->macro_expension = (char**)xcalloc(INIT_PHY_SZ, sizeof(char*));
    new_node->next = NULL;

    return new_node;
}

void expand_macro_to_file(FILE* out, MacroList* list, char* name)
{
    int i;
    MacroListNode* head = list->head;

    while (head) {
        if (strcmp(head->macro_name, name) == 0) {
            for (i = 0; i < head->log_sz; i++) {
                fputs(head->macro_expension[i], out);
                fputc('\n', out);
            }
        }
        head = head->next;
    }
}

void macro_list_insert_node(MacroList* list, MacroListNode* node)
{
    if (macro_list_is_empty(list)) {
        list->head = list->tail = node;
    }
    else {
        list->tail->next = node;
        list->tail = list->tail->next;
    }
}

void macro_list_node_insert_source(MacroListNode* node, char* line)
{
    size_t text_length = strlen(line);

    if (node->log_sz + 1 >= node->phy_sz) {
        GROW_CAPACITY(node->phy_sz);
        node->macro_expension = GROW_ARRAY(char**, node->macro_expension, node->phy_sz, sizeof(char*));
    }

    node->macro_expension[node->log_sz] = (char*)xcalloc(text_length + 1, sizeof(char));
    sprintf(node->macro_expension[node->log_sz], "%s", line);
    node->log_sz++;
}

void macro_list_node_insert_macro(MacroListNode* tail, MacroListNode* node)
{
    int i;
    for (i = 0; i < node->log_sz; i++) {
        macro_list_node_insert_source(tail, node->macro_expension[i]);
    }
}

MacroListNode* macro_list_get_node(MacroList* list, char* entry)
{
    MacroListNode* head = list->head;

    while (head) {
        if (strcmp(head->macro_name, entry) == 0) return head;
        head = head->next;
    }

    return NULL;
}

void create_pre_assembler_file(FILE* in, FILE* out, MacroList* list)
{
    char* line, * name = NULL;
    LineIterator it;
    ReadState current_state = READ_UNKNOWN;
    bool did_started_reading = FALSE;

    while ((line = get_line(in)) != NULL) {
        name = NULL;
        line_iterator_put_line(&it, line);

        if (line_iterator_peek(&it) == '\0') {
            free(line);
            continue;
        }

        /* If blanks are encountered, consume them. */
        line_iterator_consume_blanks(&it);

        /* Get the current state. */
        current_state = get_current_reading_state(&it);

        /* Skip comments, and if not a comment, copy macro (if the state is correct). */
        if (current_state != READ_COMMENT) {
            name = get_macro_name(&it);

            /* Check wheter we encountered a valid macro name, and we did not started reading a macro. */
            if (macro_list_get_node(list, name) && !did_started_reading) {
                /* If the state is READ_START_MACRO, change the flag to reflect that we are inside a macro definition. */
                if (current_state == READ_START_MACRO) {
                    did_started_reading = TRUE;
                }
                else {
                    /* Expand the macro.*/
                    expand_macro_to_file(out, list, name);
                }
            }
            /* We reached an 'endmcr' thus the macro defintion has ended. Change the flag to reflect that. */
            else if (current_state == READ_END_MACRO) {
                did_started_reading = FALSE;
            }
            else {
                /*
                    If we reached the end of a macro definitons, and we are not currently reading a macro.
                    So the macro wont be copied twice.
                */
                if (!did_started_reading) {
                    fputs(line, out);
                    fputc('\n', out);
                }
            }
        }

        free(name);
        free(line);
    }
}

void macro_free_expension(char*** macro_expension, int size)
{
    char** ptr = *macro_expension;
    int i;

    for (i = 0; i < size; i++) {
        free(ptr[i]);
    }

    free(ptr);
}

void macro_list_free(MacroList** list)
{
    MacroListNode* next, * current = (*list)->head;

    while (current) {
        next = current->next;
        free(current->macro_name);
        macro_free_expension(&current->macro_expension, current->log_sz);
        free(current);
        current = next;
    }
    free(*list);
}
