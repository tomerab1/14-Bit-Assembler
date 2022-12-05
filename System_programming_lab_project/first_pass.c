#include "first_pass.h"
#include "syntactical_analysis.h"
#include <string.h>

typedef bool (*fpass_dispatch_table)(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, const char* name, long line);

bool do_first_pass(const char* path, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list)
{
	FILE* in = open_file(path, MODE_READ);
	LineIterator it;
	char* curr_line = NULL, *next = NULL;
	long line = 1;
	bool did_error_occurred = FALSE;
	errorCodes err_code = FALSE;

	fpass_dispatch_table table[FP_TOTAL] = {
		first_pass_process_sym_def,
		first_pass_process_sym_data,
		first_pass_process_sym_string,
		first_pass_process_sym_ext,
		first_pass_process_sym_ent,
		first_pass_process_opcode
	};


	while ((curr_line = get_line(in)) != NULL) {
		char* word = NULL;
		
		/* Feed the iterator with a new line. */
		line_iterator_put_line(&it, curr_line);
		/* Trim white spaces. */
		line_iterator_consume_blanks(&it);

		word = line_iterator_next_word(&it);
		firstPassStates state = get_symbol_type(&it, word);

		/* e.g MAIN .extern/entry LOOP, MAIN will be ignored. */
		if (state == FP_SYM_IGNORED) {
			debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, ERROR_CODE_SYMBOL_IGNORED_WARN));
		}
		/* none of the above, must be an error. */
		else if (state == FP_NONE) {
			debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, ERROR_CODE_SYNTAX_ERROR));
		}
		else {
			did_error_occurred |= table[state](&it, img, sym_table, dbg_list, word, line);
		}

		free(word);
		line++;
	}

	return did_error_occurred;
}

firstPassStates get_symbol_type(LineIterator* it, char* word)
{
	bool is_valid = TRUE;

	/* An .entry definition. */
	if (strcmp(word, ".entry") == 0) {
		return FP_SYM_ENT;
	}
	/* An .extern definition. */
	else if (strcmp(word, ".extern") == 0) {
		return FP_SYM_EXT;
	}
	else if (get_opcode(word) != OP_UNKNOWN) {
		/* Unget the opcode. */
		line_iterator_unget_word(it, word);
		return FP_OPCODE;
	}
	/* Symbol definition, may follow, .data or .string*/
	else if ((is_valid = is_valid_label(word)) == TRUE) {
		const char* next_word = line_iterator_next_word(it);
		/* Check if .data */
		if (strcmp(next_word, ".data") == 0) {
			return FP_SYM_DATA;
		}
		/* Check if .string */
		if (strcmp(next_word, ".string") == 0) {
			return FP_SYM_STR;
		}
		if (strcmp(next_word, "entry") == 0 || strcmp(next_word, "extern") == 0) {
			line_iterator_unget_word(it, next_word);
			line_iterator_unget_word(it, word);
			return FP_SYM_IGNORED;
		}

		/* Unget the word, and return FP_SYM_DEF */
		line_iterator_unget_word(it, next_word);
		return FP_SYM_DEF;
	}

	if (!is_valid) {
		return FP_NONE;
	}


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
		puts(word);
	}
}

bool first_pass_process_sym_def(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, const char* name, long line)
{
	if (symbol_table_search_symbol(sym_table, name)) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYMBOL_REDEFINITION));
		return FALSE;
	}
	symbol_table_insert_symbol(sym_table, symbol_table_new_node(name, SYM_CODE, img->instruction_image.counter));

	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	if (!validate_syntax(*it, FP_SYM_DEF, line, dbg_list)) {
		return FALSE;
	}
	build_memory_word(it, img, dbg_list);
	return TRUE;
}

bool first_pass_process_opcode(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, const char* name, long line)
{
	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	if (!validate_syntax(*it, FP_OPCODE, line, dbg_list)) {
		return FALSE;
	}
	build_memory_word(it, img, dbg_list);
	return TRUE;
}


bool first_pass_process_sym_data(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, const char* name, long line)
{
	if (symbol_table_search_symbol(sym_table, name)) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYMBOL_REDEFINITION));
		return FALSE;
	}
	symbol_table_insert_symbol(sym_table, symbol_table_new_node(name, SYM_DATA, img->data_image.counter));

	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	if (!validate_syntax(*it, FP_SYM_DATA, line, dbg_list)) {
		return FALSE;
	}
	build_memory_word(it, img, dbg_list);
	return TRUE;
}

bool first_pass_process_sym_string(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, const char* name, long line)
{
	if (symbol_table_search_symbol(sym_table, name)) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYMBOL_REDEFINITION));
		return FALSE;
	}
	symbol_table_insert_symbol(sym_table, symbol_table_new_node(name, SYM_DATA, img->data_image.counter));

	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	if (!validate_syntax(*it, FP_SYM_STR, line, dbg_list)) {
		return FALSE;
	}
	build_memory_word(it, img, dbg_list);
	return TRUE;
}

bool first_pass_process_sym_ent(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, const char* name, long line)
{
	const char* word = line_iterator_next_word(it);

	line_iterator_unget_word(it, word);
	if (!word) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYNTAX_ERROR));
		free(word);
		return FALSE;
	}
	if (!is_label_name(it)) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_LABEL_DEF));
		free(word);
		return FALSE;
	}
	if (symbol_table_search_symbol(sym_table, word)) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYMBOL_REDEFINITION));
		free(word);
		return FALSE;
	}

	symbol_table_insert_symbol(sym_table, symbol_table_new_node(word, SYM_ENTRY, 0));

	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	if (!validate_syntax(*it, FP_SYM_ENT, line, dbg_list)) {
		return FALSE;
	}
	
	build_memory_word(it, img, dbg_list);
	free(word);

	return TRUE;
}

bool first_pass_process_sym_ext(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, const char* name, long line)
{
	const char* word = line_iterator_next_word(it);

	line_iterator_unget_word(it, word);
	if (!word) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYNTAX_ERROR));
		free(word);
		return FALSE;
	}
	if (!is_label_name(it)) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_LABEL_DEF));
		free(word);
		return FALSE;
	}
	if (symbol_table_search_symbol(sym_table, word)) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYMBOL_REDEFINITION));
		free(word);
		return FALSE;
	}

	symbol_table_insert_symbol(sym_table, symbol_table_new_node(word, SYM_EXTERN, 0));

	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	if (!validate_syntax(*it, FP_SYM_ENT, line, dbg_list)) {
		return FALSE;
	}

	build_memory_word(it, img, dbg_list);
	free(word);

	return TRUE;
}