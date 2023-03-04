#ifndef FIRST_PASS_H
#define FIRST_PASS_H

/** @file
*/

#include "symbol_table.h"7
#include "line_iterator.h"
#include "utils.h"
#include "memory.h"
#include "debug.h"

/**
* @brief An enum for the different states of the first pass algorithm.
*/
typedef enum { FP_SYM_DEF, FP_SYM_DATA, FP_SYM_STR, FP_SYM_EXT, FP_SYM_ENT, FP_OPCODE, FP_TOTAL, FP_NONE, FP_SYM_IGNORED } firstPassStates;

/** 
* @brief This function implements the first pass algorithm.
* @param path - The path to the pre-assembled file.
* @param img - A pointer to the memory buffer, contains the data/instruction img and the registers.
* @param sym_table - A pointer to the symbol table.
* @param dbg_list - A pointer to the debug list, used to register errors.
* @return - TRUE if no errors occurred, FALSE otherwise.
*/
bool do_first_pass(char* path, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list);

/** 
 * @brief This function take in a string, and checks if it's a symbol, if so it returns it's type.
 * This function also checks if the symbol name is a valid symbol name.
 * @param it - A string to do the check upon.
 * @return A appropriate firstPassState.
*/
firstPassStates get_symbol_type(LineIterator* it, char* word);

/**
* @brief This function is used to process lines with label definitions.
* It also encodes the memory if possible into the memoryBuffer and updates the symbol table if necessary.
* @param it - The line interator.
* @param img - The memory buffer.
* @param sym_table - The symbol table.
* @param dbg_list - The debug list.
* @param name - The name of a label.
* @param line - The current line.
* param should_encode - If there is an error we can skip the encoding phase.
* @return True if there are no errors, false otherwise.
*/
bool first_pass_process_sym_def(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode);

/**
* @brief This function is used to process .entry lines.
* It also encodes the memory if possible into the memoryBuffer and updates the symbol table if necessary.
* @param it - The line interator.
* @param img - The memory buffer.
* @param sym_table - The symbol table.
* @param dbg_list - The debug list.
* @param name - The name of a label.
* @param line - The current line.
* param should_encode - If there is an error we can skip the encoding phase.
* @return True if there are no errors, false otherwise.
*/
bool first_pass_process_sym_ent(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode);

/**
* @brief This function is used to process .string lines.
* It also encodes the memory if possible into the memoryBuffer and updates the symbol table if necessary.
* @param it - The line interator.
* @param img - The memory buffer.
* @param sym_table - The symbol table.
* @param dbg_list - The debug list.
* @param name - The name of a label.
* @param line - The current line.
* param should_encode - If there is an error we can skip the encoding phase.
* @return True if there are no errors, false otherwise.
*/
bool first_pass_process_sym_string(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode);

/**
* @brief This function is used to process .data lines.
* It also encodes the memory if possible into the memoryBuffer and updates the symbol table if necessary.
* @param it - The line interator.
* @param img - The memory buffer.
* @param sym_table - The symbol table.
* @param dbg_list - The debug list.
* @param name - The name of a label.
* @param line - The current line.
* param should_encode - If there is an error we can skip the encoding phase.
* @return True if there are no errors, false otherwise.
*/
bool first_pass_process_sym_data(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode);

/**
* @brief This function is used to process .extern lines.
* It also encodes the memory if possible into the memoryBuffer and updates the symbol table if necessary.
* @param it - The line interator.
* @param img - The memory buffer.
* @param sym_table - The symbol table.
* @param dbg_list - The debug list.
* @param name - The name of a label.
* @param line - The current line.
* param should_encode - If there is an error we can skip the encoding phase.
* @return True if there are no errors, false otherwise.
*/
bool first_pass_process_sym_ext(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode);

/**
* @brief This function is used to process lines with opcodes and no label definitions.
* It also encodes the memory if possible into the memoryBuffer and updates the symbol table if necessary.
* @param it - The line interator.
* @param img - The memory buffer.
* @param sym_table - The symbol table.
* @param dbg_list - The debug list.
* @param name - The name of a label.
* @param line - The current line.
* param should_encode - If there is an error we can skip the encoding phase.
* @return True if there are no errors, false otherwise.
*/
bool first_pass_process_opcode(LineIterator* it, memoryBuffer* img, SymbolTable* sym_table, debugList* dbg_list, char* name, long line, bool should_encode);

#endif