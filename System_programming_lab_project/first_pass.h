#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "syntactical_analysis.h"
#include "symbol_table.h"
#include "line_iterator.h"
#include "utils.h"
#include "memory.h"
#include "debug.h"

typedef enum { FP_NONE, FP_SYM_DEF, FP_SYM_DATA, FP_SYM_STR, FP_SYM_EXT, FP_SYM_ENT, FP_OPCODE, FP_SYM_IGNORED } firstPassStates;


/* This function implements the first pass algorithm.
* @param - The path to the pre-assembled file.
* @param - A pointer to the memory buffer, contains the data/instruction img and the registers.
* @param - A pointer to the symbol table.
* @param - A pointer to the debug list, used to register errors.
* @return - TRUE if no errors occurred, FALSE otherwise.
*/
bool do_first_pass(const char* path, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list);

/* This function take in a string, and checks if it's a symbol, if so it returns it's type.
 * This function also checks if the symbol name is a valid symbol name.
 * @param - A string to do the check upon.
 * @return - A appropriate firstPassState.
*/
firstPassStates get_symbol_type(LineIterator* it, char* word);

/* This function calls to function that process and incode the instructions.
*/
bool first_pass_process_and_encode_instructions(LineIterator* it, memoryBuffer* img, symbolType* sym_table, debugList* dbg_list);

/* Function for encoding the instructions to memory words.
*/
void build_memory_word(LineIterator* it, memoryBuffer* img, debugList* dbg_list);

bool first_pass_process_sym_def(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, const char* name, long line);


bool first_pass_process_sym_ent(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, const char* name, long line);


bool first_pass_process_sym_string(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, const char* name, long line);


bool first_pass_process_sym_data(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, long line);


bool first_pass_process_sym_ext(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, long line);


bool first_pass_process_instruction(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, long line);


bool first_pass_is_instruction(LineIterator* it, SymbolTable* sym_table, debugList* dbg_list, long line);

#endif