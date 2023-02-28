/** @file
*/

#include "first_pass.h"
#include "syntactical_analysis.h"
#include "encoding.h"
#include <string.h>

typedef bool (*fpass_dispatch_table)(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool did_err_occurred);

/**
* This function is called by fpass_decode. The file is read from path and each line of the file is examined for instructions that need to be encoded.
* 
* @param path
* @param img
* @param sym_table
* @param dbg_list - The debug list to be used for decoding symbols.
* 
* @return TRUE if the encoding should be done FALSE otherwise. In this case the caller should allocate memory for the image
*/
/**
* This function is called by fpass_decode. The file is read from the path and each line is processed to determine if it should be encoded.
* 
* @param path
* @param img
* @param sym_table
* @param dbg_list - The debug list to be used for decoding the file.
* 
* @return TRUE if encoding should be done FALSE otherwise. In this case the caller should allocate memory for the image
*/
bool do_first_pass(char* path, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list)
{
	FILE* in = open_file(path, MODE_READ);
	LineIterator it;
	char* curr_line = NULL;
	long line = 1;
	bool should_encode = TRUE;

	fpass_dispatch_table table[FP_TOTAL] = {
		first_pass_process_sym_def,
		first_pass_process_sym_data,
		first_pass_process_sym_string,
		first_pass_process_sym_ext,
		first_pass_process_sym_ent,
		first_pass_process_opcode
	};

	// Read a new line from the input stream.
	while ((curr_line = get_line(in)) != NULL) {
		char* word = NULL;
		
		/* Feed the iterator with a new line. */
		line_iterator_put_line(&it, curr_line);
		/* Trim white spaces. */
		line_iterator_consume_blanks(&it);

		word = line_iterator_next_word(&it, " ");
		firstPassStates state = get_symbol_type(&it, word);

		if (state == FP_SYM_IGNORED) {
			debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, ERROR_CODE_SYMBOL_IGNORED_WARN));
		}
		/* none of the above, must be an error. */
		/* if state FP_NONE register the node in the list and register it as a new node. */
		else if (state == FP_NONE) {
			debug_list_register_node(dbg_list, debug_list_new_node(it.start, it.current, line, ERROR_CODE_SYNTAX_ERROR));
			should_encode = FALSE;
		}
		else {
			should_encode &= table[state](&it, img, sym_table, dbg_list, word, line, should_encode);
		}

		free(word);
        free(curr_line);
		line++;
	}

    fclose(in);
	sym_table->completed = TRUE;
	return should_encode;
}

/**
* Get the type of a symbol. This is a bit tricky because we don't know what kind of symbol it is.
* 
* @param it
* @param word
* 
* @return A firstPassStates value indicating the state of the parsing
*/
firstPassStates get_symbol_type(LineIterator* it, char* word)
{
	bool is_valid = TRUE;

	/* An .entry definition. */
	// Returns FP_SYM_ENT or FP_SYM_EXT. entry. extern. data or. string.
	if (strcmp(word, ".entry") == 0) {
		return FP_SYM_ENT;
	}
	/* An .extern definition. */
	// Returns FP_SYM_EXT or FP_SYM_DEF depending on the word.
	else if (strcmp(word, ".extern") == 0) {
		return FP_SYM_EXT;
	}
	// Get the opcode of the word.
	else if (get_opcode(word) != OP_UNKNOWN) {
		/* Unget the opcode. */
		line_iterator_unget_word(it, word);
		return FP_OPCODE;
	}
	/* Symbol definition, may follow, .data or .string*/
	// Check if the word is a valid label.
	else if ((is_valid = is_valid_label(word)) == TRUE) {
		char* next_word = line_iterator_next_word(it, " ");

		// Free the next word.
		if (!next_word) {
			free(next_word);
			return FP_NONE;
		}

		/* Check if .data */
		if (strcmp(next_word, ".data") == 0) {
            free(next_word);
			return FP_SYM_DATA;
		}
		/* Check if .string */
		if (strcmp(next_word, ".string") == 0) {
            free(next_word);
			return FP_SYM_STR;
		}
		/* Ungets the next word from the line iterator. */
		if (strcmp(next_word, "entry") == 0 || strcmp(next_word, "extern") == 0) {
			line_iterator_unget_word(it, next_word);
			line_iterator_unget_word(it, word);
            free(next_word);
			return FP_SYM_IGNORED;
		}

		/* Unget the word, and return FP_SYM_DEF */
		line_iterator_unget_word(it, next_word);
        free(next_word);
		return FP_SYM_DEF;
	}

	/* Returns FP_NONE if the field is not valid. */
	if (!is_valid) {
		return FP_NONE;
	}

	return FP_NONE;
}

/**
* Process a symbol definition. This involves creating a node to represent the symbol and inserting it into the symbol table.
* 
* @param it
* @param img
* @param sym_table
* @param dbg_list - Debug list to add the debug information to.
* @param name - Name of the symbol to process. This must be a pointer to a null terminated string.
* @param line - Line number where the symbol definition starts. This is used for error reporting.
* @param should_encode - True if the symbol should be encoded.
* 
* @return TRUE if the symbol definition was processed successfully FALSE otherwise. In this case an error is reported to the user
*/
bool first_pass_process_sym_def(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode)
{
	/* Get a handle to the node, if the type is entry/extern then update its counter to the img->instruction_image.counter. */
	/* If it is not an extern/entry then register an error. */
	SymbolTableNode* node = symbol_table_search_symbol(sym_table, name);

	// Register a symbol definition node.
	if (node && (node->sym.type == SYM_DATA || node->sym.type == SYM_CODE)) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYMBOL_REDEFINITION));
		return FALSE;
	}

	symbol_table_insert_symbol(sym_table, symbol_table_new_node(name, SYM_CODE, img->instruction_image.counter));

	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	// Check if the syntax is valid.
	if (!validate_syntax(*it, FP_SYM_DEF, line, dbg_list)) {
		return FALSE;
	}
	// Encode to the image.
	if (should_encode) {
		encode_opcode(it, img);
	}

	return TRUE;
}

/**
* Process an opcode and encode it if should_encode is true. This is the first pass of the machine learning algorithm.
* 
* @param it
* @param img
* @param sym_table
* @param dbg_list - Debug list for the machine learning algorithm
* @param name - Name of the machine learning algorithm ( used for error reporting )
* @param line - Line number of the opcode. Can be - 1 if unknown
* @param should_encode - Flag to indicate if the opcode should be encoded
* 
* @return TRUE if the iterator
*/
bool first_pass_process_opcode(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode)
{
	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	if (!validate_syntax(*it, FP_OPCODE, line, dbg_list)) {
		return FALSE;
	}
	// Encode to the image.
	if (should_encode) {
		encode_opcode(it,img);
	}
	return TRUE;
}


/**
* Process a symbol that is an entry in the symbol table. This involves creating a node to represent the symbol and inserting it into the symbol table.
* 
* @param it
* @param img
* @param sym_table
* @param dbg_list - The debug list to add the debug info to
* @param name - The name of the symbol to process ( used for error reporting )
* @param line - The line number on which the symbol resides
* @param should_encode - True if the symbol should be encoded
* 
* @return TRUE if the symbol was added FALSE if it was redefinted in which case an error has been
*/
bool first_pass_process_sym_data(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode)
{
	/* Get a handle to the node, if the type is entry/extern then update its counter to the img->instruction_image.counter. */
	/* If it is not an extern/entry then register an error. */
	SymbolTableNode* node = symbol_table_search_symbol(sym_table, name);

	if (node && (node->sym.type == SYM_DATA || node->sym.type == SYM_CODE)) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYMBOL_REDEFINITION));
		return FALSE;
	}

	symbol_table_insert_symbol(sym_table, symbol_table_new_node(name, SYM_DATA, img->instruction_image.counter + img->data_image.counter));

	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	if (!validate_syntax(*it, FP_SYM_DATA, line, dbg_list)) {
		return FALSE;
	}
	// Encode the .data.
	if (should_encode) {
		encode_dot_data(it, img);
	}

	return TRUE;
}

/**
* Process a symbol string. This involves verifying the syntax and inserting the symbol into the symbol table
* 
* @param it
* @param img
* @param sym_table
* @param dbg_list - Debug list for the current file
* @param name
* @param line - Line number of the symbol string we are looking for
* @param should_encode - Whether or not we should encode the string
* 
* @return TRUE on success FALSE on error ( symbol already exists or syntax is wrong ) Note that this is a first pass process
*/
bool first_pass_process_sym_string(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode)
{
	if (symbol_table_search_symbol_bool(sym_table, name)) {
		debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYMBOL_REDEFINITION));
		return FALSE;
	}
	symbol_table_insert_symbol(sym_table, symbol_table_new_node(name, SYM_DATA, img->data_image.counter));

	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	// Check if the syntax is valid.
	if (!validate_syntax(*it, FP_SYM_STR, line, dbg_list)) {
		return FALSE;
	}
	// Encode the image as a dot string.
	if (should_encode) {
		encode_dot_string(it, img);
	}
	return TRUE;
}

/**
* Process a symbol entry. This is the first pass of the debug_list_process_symtab function.
* 
* @param it
* @param img
* @param sym_table
* @param dbg_list - Debug list to add nodes to.
* @param name - Name of the symbol entry. Can be NULL in which case a name will be generated.
* @param line - Line number in the file where the symbol entry resides.
* @param should_encode - Flag indicating if the encoding should be done.
* 
* @return TRUE on success FALSE on failure ( and errno set ). If this is FALSE the iterator is positioned on an error
*/
bool first_pass_process_sym_ent(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode)
{
    char* word = line_iterator_next_word(it, " ");

	line_iterator_unget_word(it, word);
	// register a new word in the list
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

	/* Check wheter the symbol already exist as an entry/extern directive */
	SymbolTableNode* node = symbol_table_search_symbol(sym_table, word);

	// Insert symbol in symbol table.
	if (node && (node->sym.type != SYM_ENTRY && node->sym.type != SYM_EXTERN)) {
		symbol_table_insert_symbol(sym_table, symbol_table_new_node(word, SYM_ENTRY, node->sym.counter));
	}
	else {
		symbol_table_insert_symbol(sym_table, symbol_table_new_node(word, SYM_ENTRY, 0));
	}

	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	// Check if the syntax is valid.
	if (!validate_syntax(*it, FP_SYM_ENT, line, dbg_list)) {
		free(word);
		return FALSE;
	}

	free(word);

	return TRUE;
}

/**
* Process a symbol that starts with " extern ". This is used to determine if we are going to encode or unencod the symbol.
* 
* @param it
* @param img
* @param sym_table
* @param dbg_list - Debug list to add nodes to
* @param name - Name of the symbol ( can be NULL )
* @param line - Line number of the symbol ( 0 for unknown )
* @param should_encode - Flag to indicate if the symbol should be encoded
* 
* @return TRUE on success FALSE on error ( in which case the iterator is left pointing at the error message )
*/
bool first_pass_process_sym_ext(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode)
{
    char* word = line_iterator_next_word(it, " ");

	line_iterator_unget_word(it, word);
	// register a new word in the list
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

	symbol_table_insert_symbol(sym_table, symbol_table_new_node(word, SYM_EXTERN, 0));

	/* Check the syntax, we want a copy of the iterator because if the syntax is correct we will encode the instructions to memory. */
	// Check if the syntax is valid.
	if (!validate_syntax(*it, FP_SYM_ENT, line, dbg_list)) {
		free(word);
		return FALSE;
	}

	free(word);

	return TRUE;
}