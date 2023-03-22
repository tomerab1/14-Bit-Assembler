#ifndef SECOND_PASS_H
#define SECOND_PASS_h

/** @file
*/

#include "encoding.h"

/*
* @brief A structure which indicates wheter .extern or/and .entry files exists, so corresponding files will be created.
*/
typedef struct flags flags;

/**
@brief A structure representing a translated machine code instruction, which being printed in the end of the prorgam @ object file
*/
typedef struct TranslatedMachineData TranslatedMachineData;

/**
@brief A structure representing the final status of the program, being updated during second pass.
*/
typedef struct programFinalStatus programFinalStatus;


/**
@brief Initiates the second pass of the assembler.
This function is responsible for executing the second pass of the assembly process. It reads each line from the input file,
sets a base address for the symbol table, initializes the instruction image counter to 0, and then executes the line by
calling execute_line() if it's not a directive or extracts the directive type if it is. Finally, it creates output files
for the assembly code.
@param path The path of the input file.
@param table A pointer to the symbol table.
@param memory A pointer to the memory buffer.
@param dbg_list A pointer to the debug list.
@return TRUE if the function executed successfully, FALSE otherwise.
*/
bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory, debugList* dbg_list);

/**
 * @brief Generates an object file from the data in a memory buffer.
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

/**
@brief Translates the content of a memory buffer to machine code.
@param memory A pointer to the memory buffer to be translated.
@return TranslatedMachineData* A pointer to a translated machine data struct.
The struct contains the machine code equivalent of the given memory buffer.
Returns NULL if the memory buffer is empty or cannot be translated.
*/
TranslatedMachineData* translate_to_machine_data(memoryBuffer* memory);

/**
@brief Generates an externals file containing the names and addresses of external symbols
@param table Pointer to the symbol table containing the external symbols
@param path Pointer to the path of the original source file
@return TRUE if the file was generated successfully, FALSE otherwise
*/
bool generate_externals_file(SymbolTable* table, char* path);

/**
@brief Generates an entries file containing the entry symbols and their addresses
The function generates an entries file by iterating over the symbol table nodes and
writing to file the name and address of each symbol with type SYM_ENTRY.
@param table Pointer to the symbol table
@param path Path to the source file
@return Returns TRUE if the file generation was successful, FALSE otherwise
*/
bool generate_entries_file(SymbolTable* table, char* path);

/**
@brief Creates output files based on the given memory buffer, path, final program status, and symbol table.
@param memory Pointer to the memory buffer containing instruction and data images
@param path Path to the file without the extension
@param finalStatus Pointer to a programFinalStatus struct to update the status of the program's output files
@param table Pointer to a SymbolTable struct representing the symbol table of the program
*/
void create_files(memoryBuffer* memory, char* path, programFinalStatus* finalStatus, SymbolTable* table);

/**
 * @brief Sets the dot_extern_exists flag in the given flags struct to TRUE.
 * @param flag Pointer to the flags struct to modify.
 */
void extern_exists(flags* flag);

/**
 * @brief Sets the dot_entry_exists flag in the given flags struct to TRUE.
 * @param flag Pointer to the flags struct to modify.
 */
void entry_exists(flags* flag);

/**

@brief Extracts the directive type from a given line and sets the appropriate flag
@param line Pointer to a LineIterator object representing the line to extract the directive type from
@param flag Pointer to a flags object to set the appropriate flag according to the directive type
*/
void extract_directive_type(LineIterator* line, flags* flag);

/**
@brief Executes the line, in case label exists finds if it is valid and then encodes it, otherwise prints error or skips cells.
@param it a pointer to the line iterator
@param table a pointer to the symbols table
@param memory a pointer to the memeory
@param dbg_list a pointer to the debug list, which prints the error after program stops
@param errorFlag turns true in case when error found, so files won't be created
@param line_num used to indicates the line where error has been occured
*/
void execute_line(LineIterator* it, SymbolTable* table, memoryBuffer* memory, debugList* dbg_list, bool* errorFlag, long line_num);

/**
@brief Counts the amount of lines without any labels, which are needed to be skiped, and updates the original memory
@param memory Pointer to the memory
@param it Pointer to the line iterator
*/
void skip_first_pass_mem(memoryBuffer* memory, LineIterator* it);

/**
@brief Counts the amount of lines without any labels, which are needed to be skiped.
@param it Pointer to the line iterator
*/
int find_amount_of_lines_to_skip(LineIterator* it);

/**
@brief Checks whether a label exists in a line and is defined in the symbol table.
@param line Pointer to a LineIterator struct containing the line to check.
@param table Pointer to a SymbolTable struct containing the symbol table.
@param dbg_list Pointer to a debugList struct used for debug prints.
@param flag Pointer to a boolean flag used for error handling.
@param line_num The number of the line being checked.
@return True if the label exists and is defined in the symbol table, false otherwise.
*/
bool is_label_exists_in_line(LineIterator* line, SymbolTable* table, debugList* dbg_list, bool* flag, long line_num);

/**

@brief Check if a label exists in the given line and update debug information accordingly.
@param line The line iterator.
@param table The symbol table.
@param dbg_list The debug list.
@param flag A boolean pointer to indicate if an error occurred.
@param line_num The current line number.
@return A boolean indicating if a label exists in the given line.
*/
bool investigate_word(LineIterator* originalLine, LineIterator* wordIterator, SymbolTable* table, debugList* dbg_list, bool* flag, long line_num, char* wordToInvestigate, int amoutOfVars);

/**

@brief Finds the starting point of a given word within a line.
This function takes a LineIterator object, a string of characters representing
the word to search for, and an integer representing the amount of variables
in the line. It locates the starting point of the given word within the line,
taking into account the amount of variables in the line.
@param it The LineIterator object representing the line to search.
@param word A string representing the word to search for within the line.
@param amountOfVars An integer representing the amount of variables in the line.
*/
void find_word_start_point(LineIterator* it, char* word, int amountOfVars);

/**
 * @brief Updates the addresses of symbols in the given memory buffer and symbol table
 * based on the given assembly line iterator.
 *
 * @param it The assembly line iterator to extract symbols from.
 * @param memory The memory buffer to update symbols in.
 * @param table The symbol table to update symbols in.
 */
void update_symbol_address(LineIterator it, memoryBuffer* memory, SymbolTable* table);

/**
@brief Updates the symbol offset in memory.
This function iterates through all symbols in a symbol table using a linked list traversal macro. For each symbol,
if its type is either SYM_DATA or SYM_CODE, the function adds the decimal address base value to its counter field.
This is used in assembly programming to set the base address of the program's memory space.
@param table A pointer to the symbol table.
*/
void update_symbol_offset(char* word, int offset, memoryBuffer* memory, SymbolTable* table);

/**
@brief Adds the decimal base address to all data and code symbols in a symbol table.
This function iterates through all symbols in a symbol table using a linked list traversal macro. For each symbol,
if its type is either SYM_DATA or SYM_CODE, the function adds the decimal address base value to its counter field.
This is used in assembly programming to set the base address of the program's memory space.
@param table A pointer to the symbol table.
*/
void add_label_base_address(SymbolTable* table);

/**
@brief Decodes memory so it would be suitable for printing to the object file.
@param tmd Pointer to the translated memory structure contains the memory suitable for printing
@param inst pointer to the memory used during the program
@param startPos starting position of the memory at the start of func
@param endPos ending position of the memory counter during the end of func
*/
void decode_memory(TranslatedMachineData* tmd, imageMemory* inst, int* startPos, int endPos);

/**

@brief Extracts the variables from the current line of assembly code.
This function extracts the variables from the current line of assembly code based on the opcode and syntax group
of the instruction. It returns a pointer to the extracted variables as a VarData struct.
@param it The iterator for the current line of assembly code.
@return A pointer to the VarData struct containing the extracted variables, or NULL if no variables were found.
*/
VarData* extract_variables(LineIterator* it);

#endif
