/** @file
*/

#include "pre_assembler.h"
#include <string.h>
#include <ctype.h>

/**
* @brief Fills macro list from file. The file must be open for reading.
* @param in File to read from. Must be open for reading.
* @param in_list
*/
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

/**
* @brief Starts assembling pre - assembler. This involves opening the file and creating a pre - assembler file.
* @param path
*/
void start_pre_assembler(char* path)
{
    FILE* in = open_file(path, MODE_READ), * out = NULL;
    MacroList* list = macro_list_new_list();
    char* out_name = NULL;

    macro_list_fill_list_from_file(in, list);

#ifdef DEBUG
    macro_list_dump(list);
#endif

    /* Moves the file pointer back to the starting of the file. */
    rewind(in);

    out_name = get_outfile_name(path, ".am");
    out = open_file(out_name, MODE_WRITE);

    create_pre_assembler_file(in, out, list);

    /* Cleaning up. */
    macro_list_free(&list);
    free(out_name);
    fclose(out);
    fclose(in);
}

/**
* @brief Returns the state of the current reading. This is used to determine whether or not we are reading a macro or a comment.
* @param it
* @return ReadState indicating what kind of reading is happening at the current position of the line iterator. READ_UNKNOWN is returned if there is no reading
*/
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

/**
* @brief Get the name of the macro. This is used to determine the name of the macro that is to be used when calling macro_get_name ().
* @param it Iterator to the macro's line. Must be positioned at the start of the macro name.
* @return A pointer to the macro's name. It must be freed by the caller using xfree ()
*/
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

/**
* @brief Create a new list. The list is empty after this call. You must free the list when you're done with it.
* @return A pointer to the newly created list. This should be freed with macro_list_free () when no longer needed
*/
MacroList* macro_list_new_list()
{
    MacroList* new_list = (MacroList*)xmalloc(sizeof(MacroList));
    new_list->head = new_list->tail = NULL;
    return new_list;
}

/**
* @brief Check if a list is empty. This is true if the list has no elements in it and false otherwise
* @param list
* @return true if the list is empty false otherwise ( not empty or non - empty head or tail are present
*/
bool macro_list_is_empty(MacroList* list)
{
    return !list->head && !list->tail;
}

/**
* @brief Create a new node in the macro list. Allocates memory for the node and sets it up to be used by the macro_expansion function.
* @param name
* @return Pointer to the newly allocated node or NULL if there was insufficient memory to allocate the new node. Note that the pointer is valid until the next call to macro_list_next
*/
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

/**
* @brief Expand a macro to a file.
* @param out
* @param list
* @param name
*/
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

/**
* @brief Inserts a MacroListNode at the end of the list. This is useful for inserting an element into a macro list that is later used to create a macro list.
* @param list
* @param node
*/
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

/**
* @brief Insert a macro source line into the macro list.
* @param node
* @param line
*/
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

/**
* @brief Insert a macro into the macro list. This is used to insert macro expressions at the end of the macro list.
* @param tail
* @param node
*/
void macro_list_node_insert_macro(MacroListNode* tail, MacroListNode* node)
{
    int i;
    for (i = 0; i < node->log_sz; i++) {
        macro_list_node_insert_source(tail, node->macro_expension[i]);
    }
}

/**
* @brief Get the MacroListNode for a given macro name. This is used to determine if there is a macro with the given name in the macro list.
* @param list
* @param entry
* @return pointer to the macro or NULL if not found ( not an error! ) Note : the pointer is valid until the next macro_list_insert
*/
MacroListNode* macro_list_get_node(MacroList* list, char* entry)
{
    MacroListNode* head = list->head;

    while (head) {
        if (strcmp(head->macro_name, entry) == 0) return head;
        head = head->next;
    }

    return NULL;
}

/**
* @brief Creates a pre assembler file that is used to read the macros before assembly. This is a helper function for assemble_macros ().
* @param in
* @param out
* @param list
*/
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
                /* Expand the macro.*/
                expand_macro_to_file(out, list, name);
                /* If the state is READ_START_MACRO, change the flag to reflect that we are inside a macro definition. */
                if (current_state == READ_START_MACRO) {
                    did_started_reading = TRUE;
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

/**
* @brief Free memory occupied by macro expansion. This is useful for freeing memory that was allocated by macro_expension_alloc ()
* @param macro_expension Pointer to the macro expansion
* @param size Number of elements in
*/
void macro_free_expension(char*** macro_expension, int size)
{
    char** ptr = *macro_expension;
    int i;

    for (i = 0; i < size; i++) {
        free(ptr[i]);
    }

    free(ptr);
}

#ifdef  DEBUG
/**
* @brief Dump the macro list to stdout. This is useful for debugging purposes. If you want to see the contents of the macro list use macro_list_dump_contents () instead.
* @param list
*/
void macro_list_dump(MacroList* list)
{
    MacroListNode* head = list->head;
    int i;

    while (head) {
        printf("MACRO: %s\n", head->macro_name);

        for (i = 0; i < head->log_sz; i++) {
            printf("%s\n", head->macro_expension[i]);
        }

        printf("\n");
        head = head->next;
    }
}
#endif

/**
* @brief Free a macro list and all it's contents. This is useful for debugging purposes. Note that the contents of the list are untouched and should not be used after this is called
* @param list
*/
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
