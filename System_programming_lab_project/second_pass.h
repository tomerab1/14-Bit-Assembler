#ifndef SECOND_PASS_H
#define SECOND_PASS_h

/** @file
*/

#include "syntactical_analysis.h"
#include "line_iterator.h"
#include "symbol_table.h"
#include "encoding.h"
#include "memory.h"
#include "debug.h"
#include "utils.h"
#include "debug.h"
#include "encoding.h"
#include "constants.h"

typedef struct flags
{
	bool dot_entry_exists;
	bool dot_extern_exists;
} flags;

typedef struct {
	long address;
	char translated[14];
} TranslatedMachineData;

typedef struct programFinalStatus
{
	flags entryAndExternFlag;
	bool createdObject;
	bool createdExternals;
	bool createdEntry;
	debugList errors;
	bool error_flag;
} programFinalStatus;


//starts second pass process
bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory);

/**
 * Generates an object file from the data in a memory buffer.
 *
 * This function takes a `memoryBuffer` structure and a file path, and generates an object file
 * containing the data from the memory buffer. The object file is created using the `translate_to_machine_data()`
 * and `get_outfile_name()` functions, and it is written to the specified path.
 *
 * @param memory The memory buffer to generate the object file from.
 * @param path The path to the output file.
 * @param err A pointer to a `debugList` structure that will be used to store any errors that occur.
 * @return true if the object file was generated successfully, or false if an error occurred.
 */
bool generate_object_file(memoryBuffer* memory, char* path, debugList* err);

/*translates data from memory to object text style configuration*/
TranslatedMachineData* translate_to_machine_data(memoryBuffer* memory, debugList* err);

/*generates external file*/
bool generate_externals_file(SymbolTable* table, char* path);

/*generates entries file*/
bool generate_entries_file(SymbolTable* table, char* path);

/*calls file generation functions*/
void create_files(memoryBuffer* memory, char* path, programFinalStatus* finalStatus, SymbolTable* table, debugList* err);

//checks if any order type (extern or entry) commands exists in the program
bool directive_exists(LineIterator* line);

/*If extern exists changes flag to true, used later on while generating files*/
void extern_exists(flags* flag);

/*if extern exists changes flag to true, used later on while generating files*/
void entry_exists(flags* flag);

/*finds if the type of the directive*/
void extract_directive_type(LineIterator* line, flags* flag);

/*Error handling process*/
bool handle_errors(debugList* error);

/**
 * execute_line
 *
 * Executes the next line of code from the LineIterator, using the provided memoryBuffer.
 *
 * @param it Pointer to a LineIterator that contains the line of code to execute
 * @param memory Pointer to a memoryBuffer that contains memory for the line of code to use
 *
 * @return void
 */
void execute_line(LineIterator* it, SymbolTable* table, memoryBuffer* memory);

void skip_first_pass_mem(memoryBuffer* memory, LineIterator* it);

int find_amount_of_lines_to_skip(LineIterator* it);

#endif
