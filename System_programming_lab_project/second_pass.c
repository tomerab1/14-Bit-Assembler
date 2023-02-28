/** @file
*/

#include "second_pass.h"
#include "constants.h"

//error context appearances are temp 
/**
* Executes second pass of program. It is used to create symbol table and memory buffer for execution. In this second pass the file is read from file and executed in instruction_image.
* 
* @param path
* @param table
* @param memory
* 
* @return TRUE if successful FALSE
*/
bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory)
{
	FILE* in = open_file(path, MODE_READ);
	programFinalStatus finalStatus = { NULL };
	LineIterator curLine;
	char* line = NULL;

	memory->instruction_image.counter = 0; /*init IC counter*/
	while ((line = get_line(in)) != NULL) {
		bool labelFlag = FALSE; /*is current line first word is label*/
		line_iterator_put_line(&curLine, line);
		line_iterator_jump_to(&curLine, COLON_CHAR);

		if (!directive_exists(&curLine)) { /*checks if any kind of instruction exists (.something)*/
			execute_line(&curLine, table, memory);
		}
		else {
			extract_directive_type(&curLine, &finalStatus.entryAndExternFlag);
		}
	}

	/*finished reading all lines in file*/
	if (finalStatus.error_flag) {
		/* Return false. */
		handle_errors(&finalStatus.errors);
		return FALSE;
	}
	else {
		create_files(memory, path, &finalStatus, table, &finalStatus.errors);
	}

	fclose(in);
	free(line);
	return TRUE;
}

/**
* Execute a line of code. This is the entry point for the instruction and its subroutines.
* 
* @param it
* @param table
* @param memory
*/
void execute_line(LineIterator* it, SymbolTable* table, memoryBuffer* memory) {
	/* Increment counter by one, as every command has a preceding word. */
	memory->instruction_image.counter++;
	if (is_label_exists_in_line(*it, *table)) {
		/* TODO: Add a function to update the address of a label with a type
			of SYM_ENT/SYM_EXT with the address it resides in.
		*/

		update_symbol_address(*it, memory, table);
		encode_label_start_process(it, memory, table);
	}
	else 
		skip_first_pass_mem(memory, it);
}

/**
* Skips the first pass of the memory. This is used to determine how much memory we need to skip before starting a new pass.
* 
* @param memory
* @param it - The LineIterator that points to the beginning of the data
*/
void skip_first_pass_mem(memoryBuffer* memory, LineIterator* it) {
	int memCellsToJump = find_amount_of_lines_to_skip(it);
	memory->instruction_image.counter += memCellsToJump;
}

/**
* Find the amount of lines to skip. This is used to determine how many lines should be skipped in the code that does not have a register or memory operand.
* 
* @param it
* 
* @return The amount of lines to skip or 0 if there is no code to skip in this case the iterator is positioned on the start of the code
*/
int find_amount_of_lines_to_skip(LineIterator* it) {
	char* op = NULL;
	char* operand1 = NULL;
	char* operand2 = NULL;
	int total = 0;

	line_iterator_consume_blanks(it);
	op = line_iterator_next_word(it, " ");

	line_iterator_consume_blanks(it);
	operand1 = line_iterator_next_word(it, " ");

	line_iterator_consume_blanks(it);
	line_iterator_jump_to(it, COMMA_CHAR);
	operand2 = line_iterator_next_word(it, " ");
	
	if ((operand1 && operand2) && (*operand1 == REG_BEG_CHAR && *operand2 == REG_BEG_CHAR))
		return 1; /* 1 for the opcode and one for the shared memory word of 2 registers. */

	total += (operand1) ? 1 : 0;
	total += (operand2) ? 1 : 0;

	free(op);
	free(operand1);
	free(operand2);

	return total; /* 1 for the opcode, 2 for each individual memory word */
}

/**
* Generate object file. This is the entry point for the object file generation. It takes a pointer to the memory buffer that we are going to generate the object file for and the path to the file that we want to write to
* 
* @param memory
* @param path
* @param err
* 
* @return true if successful false if there was an error ( in which case err is not modified ). Note that the file is written to a file named object
*/
bool generate_object_file(memoryBuffer* memory, char* path, debugList* err)
{
	char* outfileName = NULL;
	FILE* out = NULL;
	TranslatedMachineData* translatedMemory = NULL;
	char placeholder[50] = { 0 };
	int i;

	translatedMemory = translate_to_machine_data(memory, err);

	for (i = 0; i < memory->instruction_image.counter; i++) {
		puts(translatedMemory[i].translated);
	}

	outfileName = get_outfile_name(path, ".object");
	out = open_file(outfileName, MODE_WRITE);

	sprintf(placeholder, "%9d\t%-9d\n", memory->data_image.counter, memory->instruction_image.counter);
	fputs(placeholder, out);

	for (i = 0; i < memory->instruction_image.counter; i++) {
		sprintf(placeholder, "%04d\t%s\n", translatedMemory[i].address, translatedMemory[i].translated);
		fputs(placeholder, out);
	}

	free(translatedMemory);
	free(outfileName);
	fclose(out);

	return TRUE;
}

/**
* Translates a memory buffer containing an instruction image into a TranslatedMachineData. It is assumed that the image is valid and has at least 13 bytes of data to be translated
* 
* @param memory
* @param err
* 
* @return Pointer to the translated
*/
TranslatedMachineData* translate_to_machine_data(memoryBuffer* memory, debugList* err)
{
	int i, j;
	MemoryWord* instImg = memory->instruction_image.memory;
	TranslatedMachineData* translatedMemory = (char*)xmalloc(memory->instruction_image.counter * sizeof(TranslatedMachineData));

	for (i = 0; i < memory->instruction_image.counter; i++) {
		unsigned int bits = (instImg[i].mem[1] << 0x08) | (instImg[i].mem[0]);
		translatedMemory[i].address = i;
		memset(translatedMemory[i].translated, 0, sizeof(translatedMemory[i].translated));

		for (j = 13; j >= 0; j--) {
			unsigned int mask = 1 << j;
			if ((bits & mask) != 0) {
				translatedMemory[i].translated[13 - j] = OBJECT_PRINT_SLASH;
			}
			else {
				translatedMemory[i].translated[13 - j] = OBJECT_PRINT_DOT;
			}
		}
	}

	return translatedMemory;
}

/**
* Generate a file that contains all externals. This is used to find the symbol table that has been loaded into the system and should be relocated to point to the new symbol table
* 
* @param table
* @param path
* 
* @return TRUE if successful FALSE
*/
bool generate_externals_file(SymbolTable* table, char* path) {
	char* outfileName = NULL;
	FILE* out = NULL;
	SymbolTableNode* symTableHead = table->head;
	char placeholder[20] = { 0 };

	outfileName = get_outfile_name(path, ".external");
	out = open_file(outfileName, MODE_WRITE);


	while (symTableHead != NULL) {
		if (symTableHead->sym.type == SYM_EXTERN) {
			sprintf(placeholder, "%s\t%d\n", symTableHead->sym.name, symTableHead->sym.counter);
			fputs(placeholder, out);
		}
		symTableHead = symTableHead->next;
	}

	free(outfileName);
	fclose(out);
	return TRUE;
}

/**
* Generate entries file from symbol table. This file is used to keep track of how many symbols have been used in a particular file.
* 
* @param table
* @param path
* 
* @return TRUE on success FALSE on failure. Failure can occur if there are too many entries in the symbol table
*/
bool generate_entries_file(SymbolTable* table, char* path) {
	char* outfileName = NULL;
	FILE* out = NULL;
	SymbolTableNode* symTableHead = table->head;
	char placeholder[20] = { 0 };

	outfileName = get_outfile_name(path, ".entry");
	out = open_file(outfileName, MODE_WRITE);

	while (symTableHead != NULL) {
		if (symTableHead->sym.type == SYM_ENTRY) {
			sprintf(placeholder, "%s\t%d\n", symTableHead->sym.name, symTableHead->sym.counter);
			fputs(placeholder, out);
		}
		symTableHead = symTableHead->next;
	}

	free(outfileName);
	fclose(out);
	return TRUE;
}

/**
* Create files to be used. This is the entry point for the program. It calls generate_object_file to create the object file.
* 
* @param memory - The memory buffer to use for the generation
* @param path - The path to the file to generate
* @param finalStatus - The final status to report to the user
* @param table - The symbol table to use for symbol lookup or NULL
* @param err
*/
void create_files(memoryBuffer* memory, char* path, programFinalStatus* finalStatus, SymbolTable* table, debugList* err) {
	finalStatus->createdObject = generate_object_file(memory, path, &finalStatus->errors);
	finalStatus->createdExternals = generate_externals_file(table, path);
	finalStatus->createdEntry = generate_entries_file(table, path);
}

/**
* Extracts the type of a directive and stores it in the flags. This is used to determine if we are dealing with a function or a function header
* 
* @param line
* @param flag
*/
void extract_directive_type(LineIterator* line, flags* flag) {
	char* command = line_iterator_next_word(line, " ");
	if (strcmp(command, DOT_EXTERN)) {
		extern_exists(flag);
	}
	else if (strcmp(command, DOT_ENTRY)) {
		extern_exists(flag);
	}
	else if (!(strcmp(command, DOT_STRING) || strcmp(command, DOT_DATA))) {//isn't any exists command
		debugNode err = { NULL }; //should also add debug list later on function headline
	}
	free(command);
}

/**
* Checks if a directive exists at the current position. This is used to detect if we are going to write a directive or not.
* 
* @param line
* 
* @return @c true if a directive exists @c false otherwise. The iterator is advanced past the directive's end
*/
bool directive_exists(LineIterator* line) {
	return line_iterator_word_includes(line, ".string") ||
		   line_iterator_word_includes(line, ".data") ||
		   line_iterator_word_includes(line, ".extern") ||
		   line_iterator_word_includes(line, ".entry");
}

/**
* Tell whether or not an externally defined function exists. This is used to determine whether a function is externally defined or not by examining the contents of a. EXE file.
* 
* @param flag - Pointer to struct containing information about the function to
*/
void extern_exists(flags* flag) {
	flag->dot_extern_exists = TRUE;
}

/**
* Tell the flag that we are looking for an entry. This is used to determine if a dot - entry exists or not
* 
* @param flag - The flags struct to
*/
void entry_exists(flags* flag) {
	flag->dot_entry_exists = TRUE;
}

/**
* Handles errors that occur during parsing. This is a no - op for the parser but may be used to implement more complex error handling.
* 
* @param error
* 
* @return TRUE if there were errors in the list FALSE otherwise. In this case the parser should exit with an error
*/
bool handle_errors(debugList* error) {

	return TRUE;
}

void update_symbol_address(LineIterator it, memoryBuffer* memory, SymbolTable* table)
{
	char* word;
	char* line = (char*)xcalloc(strlen(it.start) + 1, sizeof(char));
	int offset = 0;
	LineIterator cpyIt;

	strcpy(line, it.start);

	line_iterator_put_line(&cpyIt, line);
	line_iterator_replace(&cpyIt, "(), ", SPACE_CHAR);
	line_iterator_jump_to(&cpyIt, COLON_CHAR);

	while ((word = line_iterator_next_word(&cpyIt, " ")) != NULL) {
		LineIterator tmp;
		line_iterator_put_line(&tmp, word);

		if (is_label_name(&tmp)) {
			SymbolTableNode* head = table->head;

			while (head) {
				if (strcmp(head->sym.name, word) == 0 && head->sym.type == SYM_EXTERN) {
					if (head->sym.counter == 0) {
						head->sym.counter = memory->instruction_image.counter + offset - 1;
					}
					else {
						symbol_table_insert_symbol(table, symbol_table_new_node(word, SYM_EXTERN, memory->instruction_image.counter + offset - 1));
						break;
					}
				}
				else if (strcmp(head->sym.name, word) == 0 && head->sym.type == SYM_ENTRY) {
					SymbolTableNode* defHead = table->head;

					while (defHead) {
						if (strcmp(defHead->sym.name, word) == 0 && (defHead->sym.type == SYM_CODE || defHead->sym.type == SYM_DATA)) {
							head->sym.counter = defHead->sym.counter;
							break;
						}

						defHead = defHead->next;
					}
				}

				head = head->next;
			}
		}

		offset++;
		free(word);
	}

	free(line);
}