#include "first_pass.h"

bool do_first_pass(const char* path, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list)
{
	FILE* in = open_file(path, MODE_READ);
	LineIterator it;
	firstPassStates curr_state = FP_NONE;
	char* line = NULL;


	while ((line = get_line(in)) != NULL) {
		char* word = NULL;
		
		/* Feed the iterator with a new line. */
		line_iterator_put_line(&it, line);
		/* Trim white spaces. */
		line_iterator_consume_blanks(&it);

		while ((word = line_iterator_next_word(&it)) != NULL) {
			curr_state = get_symbol_type(word);
			if (curr_state == FP_SYM_DEF) { /* Found a symbol definition. */
				
			}
		}
	}

	return TRUE;
}

firstPassStates get_symbol_type(char* word)
{

	return FP_NONE;
}