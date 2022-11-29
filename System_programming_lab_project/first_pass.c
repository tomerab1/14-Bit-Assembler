#include "first_pass.h"
#include "syntactical_analysis.h"
#include <string.h>

bool do_first_pass(const char* path, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list)
{
	FILE* in = open_file(path, MODE_READ);
	LineIterator it;
	firstPassStates curr_state = FP_NONE;
	char* curr_line = NULL, *next = NULL;
	int line = 1;
	bool did_error_occurred = FALSE;
	errorCodes err_code;


	while ((curr_line = get_line(in)) != NULL) {
		char* word = NULL;
		
		/* Feed the iterator with a new line. */
		line_iterator_put_line(&it, curr_line);
		/* Trim white spaces. */
		line_iterator_consume_blanks(&it);

		while ((word = line_iterator_next_word(&it)) != NULL) {
			curr_state = get_symbol_type(word);
			/* Found a symbol definition. */
			if (curr_state == FP_SYM_DEF) {
				/* The symbol is already present in the table, register an error. */
				if (symbol_table_search_symbol(sym_table, word) != NULL) {
					debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, ERROR_CODE_SYMBOL_REDEFINITION));
					did_error_occurred = TRUE;
					break;
				}
				/* Not in table, create a new table entry. */
				else {
					/* Check if the label syntax is valid. */
					if ((err_code = check_label_syntax(word)) != ERROR_CODE_OK) {
						debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, err_code));
						did_error_occurred = TRUE;
					}
					else {
						trim_symbol_name(word);
						symbol_table_insert_symbol(sym_table, symbol_table_new_node(word, SYM_CODE, img->instruction_image.counter));
						/* Keep processing the rest of the instruction, and encode instructions in memory.*/
						did_error_occurred |= first_pass_process_and_encode_instructions(&it, img, sym_table, dbg_list);
					}
				}
			}
			else {
				/* Get the labels name. */
				next = line_iterator_next_word(&it);
				switch (curr_state) {
				case FP_SYM_STR:
				case FP_SYM_DATA:
					if (symbol_table_search_symbol(sym_table, word) == NULL) {
						debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, ERROR_CODE_SYMBOL_REDEFINITION));
						did_error_occurred = TRUE;
					}
					else {
						symbol_table_insert_symbol(sym_table, symbol_table_new_node(word, SYM_DATA, img->data_image.counter));
						/* Encode data in memory, and update the counter. */
					}
					break;
				case FP_SYM_ENT:
					if (symbol_table_search_symbol(sym_table, next) == NULL) {
						debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, ERROR_CODE_SYMBOL_REDEFINITION));
						did_error_occurred = TRUE;
					}
					symbol_table_insert_symbol(sym_table, symbol_table_new_node(next, SYM_ENTRY, img->data_image.counter));
					break;
				case FP_SYM_EXT:
					if (symbol_table_search_symbol(sym_table, next) == NULL) {
						debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, ERROR_CODE_SYMBOL_REDEFINITION));
						did_error_occurred = TRUE;
					}
					symbol_table_insert_symbol(sym_table, symbol_table_new_node(next, SYM_EXTERN, img->data_image.counter));
					break;
				default: /* Invalid symbol definition. */
					debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, ERROR_CODE_INVALID_SYM_TYPE));
					break;
				}
			}
			free(word);
		}

		line++;
	}

	return did_error_occurred;
}

void trim_symbol_name(char* sym)
{
	/* The ':' must be coupled with the name*/
	sym[strlen(sym)] = '\0';
}


firstPassStates get_symbol_type(char* word)
{
	if (strcmp(word, ".data")) return FP_SYM_DATA;
	if (strcmp(word, ".string")) return FP_SYM_STR;
	if (strcmp(word, ".extern")) return FP_SYM_EXT;
	if (strcmp(word, ".entry")) return FP_SYM_ENT;
	if (is_valid_label(word)) return FP_SYM_DEF;
	return FP_NONE;
}

bool first_pass_process_and_encode_instructions(LineIterator* it, memoryBuffer* img, symbolType* sym_table, debugList* dbg_list)
{
	bool did_error_occurred = FALSE;
	int command_len = 0;

	/* Verifies the command syntax */
	if ((did_error_occurred = verify_command_syntax(it, dbg_list)) != TRUE) {
		build_memory_word(it, img, dbg_list);
	}

	return did_error_occurred;
}

void build_memory_word(LineIterator* it, memoryBuffer* img, debugList* dbg_list)
{
	char* word = NULL;
	
	while ((word = line_iterator_next_word(it)) != NULL) {

	}
}