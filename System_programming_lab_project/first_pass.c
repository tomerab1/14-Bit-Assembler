#include "first_pass.h"

bool do_first_pass(const char* path, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list)
{
	FILE* in = open_file(path, MODE_READ);
	LineIterator it;
	firstPassStates curr_state = FP_NONE;
	char* line = NULL, *prev = NULL;
	int line = 1;
	bool did_error_occurred = FALSE;


	while ((line = get_line(in)) != NULL) {
		char* word = NULL;
		
		/* Feed the iterator with a new line. */
		line_iterator_put_line(&it, line);
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
					trim_symbol_name(word);
					symbol_table_insert_symbol(sym_table, symbol_table_new_node(word, SYM_CODE, img->instruction_image.counter));
					/* Keep processing the rest of the instruction, and encode instructions in memory.*/
					did_error_occurred = first_pass_process_and_encode_instructions(&it, img, sym_table, dbg_list);
				}
			}
			else {
				switch (get_symbol_type(word)) {
				case FP_SYM_DATA:
					symbol_table_insert_symbol(sym_table, symbol_table_new_node(word, SYM_DATA, img->data_image.counter));
					/* Encode data in memory, and update the counter. */
					break;
				case FP_SYM_ENT:
					// symbol_table_insert_all_externs(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list);
					break;
				case FP_SYM_EXT:
					break;
				default: /* Invalid symbol definition. */
					debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, ERROR_CODE_INVALID_SYM_TYPE));
					break;
				}
			}
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

	return FP_NONE;
}

bool first_pass_process_and_encode_instructions(LineIterator* it, imageMemory* img, symbolType* sym_table, debugList* dbg_list)
{
	bool did_error_occurred = FALSE;



	return did_error_occurred;
}