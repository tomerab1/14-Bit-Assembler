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
#include "constants.h"

typedef struct lines_list_node
{
	int address;
	char dataForObject[SINGLE_ORDER_SIZE]; /* 14 bits string strings. */
} LinesListNode;


typedef struct flags
{
	bool dot_entry_exists;
	bool dot_extern_exists;
} flags;

typedef struct {
	long address;
	char translated[SINGLE_ORDER_SIZE + 1]; /* (+1) for '\0'. */
} TranslatedMachineData;

typedef struct programFinalStatus
{
	flags entryAndExternFlag;
	bool createdObject;
	bool createdExternals;
	bool createdEntry;
	bool error_flag;
} programFinalStatus;


//starts second pass process
bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory, debugList* dbg_list);

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
bool generate_object_file(memoryBuffer* memory, char* path);

/*translates data from memory to object text style configuration*/
TranslatedMachineData* translate_to_machine_data(memoryBuffer* memory);

/*generates external file*/
bool generate_externals_file(SymbolTable* table, char* path);

/*generates entries file*/
bool generate_entries_file(SymbolTable* table, char* path);

/*calls file generation functions*/
void create_files(memoryBuffer* memory, char* path, programFinalStatus* finalStatus, SymbolTable* table);

/*If extern exists changes flag to true, used later on while generating files*/
void extern_exists(flags* flag);

/*if extern exists changes flag to true, used later on while generating files*/
void entry_exists(flags* flag);

/*finds if the type of the directive*/
void extract_directive_type(LineIterator* line, flags* flag);

void execute_line(LineIterator* it, SymbolTable* table, memoryBuffer* memory, debugList* dbg_list, bool* errorFlag, long line_num);

void skip_first_pass_mem(memoryBuffer* memory, LineIterator* it);

int find_amount_of_lines_to_skip(LineIterator* it);

bool is_label_exists_in_line(LineIterator* line, SymbolTable* table, debugList* dbg_list, bool* flag, long line_num);

bool investigate_word(LineIterator* originalLine, LineIterator* wordIterator, SymbolTable* table, debugList* dbg_list, bool* flag, long line_num, char* wordToInvestigate, int amoutOfVars);

void find_word_start_point(LineIterator* it, char* word, int amountOfVars);

void update_symbol_address(LineIterator it, memoryBuffer* memory, SymbolTable* table);

void update_symbol_offset(char* word, int offset, memoryBuffer* memory, SymbolTable* table);

void add_label_base_address(SymbolTable* table);

void decode_memory(TranslatedMachineData* tmd, MemoryWord* inst, int* startPos, int endPos);

#endif
