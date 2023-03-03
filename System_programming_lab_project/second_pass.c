/** @file
*/

#include "second_pass.h"
#include "constants.h"

#include <ctype.h>

/**
* Executes second pass of program. It is used to create symbol table and memory buffer for execution. In this second pass the file is read from file and executed in instruction_image.
* 
* @param path
* @param table
* @param memory
* 
* @return TRUE if successful FALSE
*/
bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory, debugList* dbg_list)
{
	FILE* in = open_file(path, MODE_READ);
	programFinalStatus finalStatus = { NULL };
	LineIterator curLine;
	char* line = NULL;

	add_label_base_address(table);
	memory->instruction_image.counter = 0; /*init IC counter*/
	
	while ((line = get_line(in)) != NULL) {
		bool labelFlag = FALSE; /*is current line first word is label*/
		line_iterator_put_line(&curLine, line);
		line_iterator_jump_to(&curLine, COLON_CHAR);

		if (!directive_exists(&curLine)) { /*checks if any kind of instruction exists (.something)*/
			execute_line(&curLine, table, memory, dbg_list, &finalStatus.error_flag, memory->instruction_image.counter);
		}
		else {
			extract_directive_type(&curLine, &finalStatus.entryAndExternFlag);
		}
	}
	if (finalStatus.error_flag) return FALSE;

	create_files(memory, path, &finalStatus, table);

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
void execute_line(LineIterator* it, SymbolTable* table, memoryBuffer* memory, debugList* dbg_list, bool* errorFlag, long line_num) {
	/* Increment counter by one, as every command has a preceding word. */
	memory->instruction_image.counter++;
	if (is_label_exists_in_line(it, table, dbg_list, errorFlag, line_num)) {
		update_symbol_address(*it, memory, table);
		encode_label_start_process(it, memory, table, dbg_list);
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
	char* op = NULL, *operand1 = NULL, *operand2 = NULL, *operand3 = NULL;
	char* tempLine = (char*)xmalloc(sizeof(char) * (strlen(it->start) + 1));
	LineIterator tempIt;
	int total = 0;

	line_iterator_put_line(&tempIt, tempLine);
	line_iterator_jump_to(&tempIt, COLON_CHAR);
	line_iterator_replace(&tempIt, "(), ", SPACE_CHAR);

	op = line_iterator_next_word(it, " ");
	operand1 = line_iterator_next_word(it, " ");
	operand2 = line_iterator_next_word(it, " ");
	operand3 = line_iterator_next_word(it, " ");
	
	/* 1 for the opcode and one for the shared memory word of 2 registers. */
	if ((operand1 && operand2) && (*operand1 == REG_BEG_CHAR && *operand2 == REG_BEG_CHAR))
		return isdigit(*(operand1 + 1)) && isdigit(*(operand2 + 1)) ? 1 : 2;

	/* 1 for the opcode and one for the shared memory word of 2 registers, and + 1 for the label name (this case is for parametrized labels) */
	if ((operand2 && operand3) && (*operand2 == REG_BEG_CHAR && *operand3 == REG_BEG_CHAR))
		return isdigit(*(operand1 + 1)) && isdigit(*(operand2 + 1)) ? 2 : 3;

	total += (operand1) ? 1 : 0;
	total += (operand2) ? 1 : 0;
	total += (operand3) ? 1 : 0;

	free(op);
	free(operand1);
	free(operand2);
	free(operand3);
	free(tempLine);

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
bool generate_object_file(memoryBuffer* memory, char* path)
{
	char* outfileName = NULL;
	FILE* out = NULL;
	TranslatedMachineData* translatedMemory = NULL;
	char placeholder[50] = { 0 };
	int i;

	translatedMemory = translate_to_machine_data(memory);

	for (i = 0; i < memory->instruction_image.counter; i++) {
		puts(translatedMemory[i].translated);
	}

	outfileName = get_outfile_name(path, ".object");
	out = open_file(outfileName, MODE_WRITE);

	sprintf(placeholder, "%9d\t%4d\n", memory->data_image.counter, memory->instruction_image.counter);
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
TranslatedMachineData* translate_to_machine_data(memoryBuffer* memory)
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
			sprintf(placeholder, "%s\t%d\n", symTableHead->sym.name, symTableHead->sym.counter+DECIMAL_ADDRESS_BASE);
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
			sprintf(placeholder, "%s\t%d\n", symTableHead->sym.name, symTableHead->sym.counter+ DECIMAL_ADDRESS_BASE);
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
void create_files(memoryBuffer* memory, char* path, programFinalStatus* finalStatus, SymbolTable* table)
{
	finalStatus->createdObject = generate_object_file(memory, path);
	table->hasExternals ? (finalStatus->createdExternals = generate_externals_file(table, path)) : NULL;
	table->hasEntries ? (finalStatus->createdEntry = generate_entries_file(table, path)) : NULL;
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

	free(command);
}

VarData extract_variables(LineIterator* it) {
	VarData variables = { 0 };

	char* opcode = line_iterator_next_word(it, " ");
	Opcodes op = get_opcode(opcode);
	SyntaxGroups synGroup = get_syntax_group(opcode);

	if (synGroup == SG_GROUP_1 || synGroup == SG_GROUP_2 || synGroup == SG_GROUP_7) {
		variables = extract_variables_group_1_and_2_and_7(it);
	}

	else if (synGroup == SG_GROUP_3 || synGroup == SG_GROUP_6) {
		variables = extract_variables_group_3_and_6(it);
	}

	else if (synGroup == SG_GROUP_5) {
		variables = extract_variables_group_5(it);
	}

	return variables;
}

bool is_label_exists_in_line(LineIterator* line, SymbolTable* table, debugList* dbg_list, bool* flag, long line_num) {
	VarData variablesData = { NULL };
	LineIterator itLeftVar, itRightVar, itLabel;
	char* tempWord = 0;
	variablesData = extract_variables(line);
	
	switch (variablesData.total)
	{
	case 1: /*group 3 and 6, left var*/
		if (variablesData.leftVar != NULL) {
			line_iterator_put_line(&itLeftVar, variablesData.leftVar);
			return investigate_word(line, &itLeftVar, table, dbg_list, flag, line_num, variablesData.leftVar, 1);
		}
		else {
			line_iterator_put_line(&itLabel, variablesData.label);
			return investigate_word(line, &itLabel, table, dbg_list, flag, line_num, variablesData.label, 1);
		}

	case 2: /*groups 1,2,7, left var and right var*/
		
		line_iterator_put_line(&itLeftVar, variablesData.leftVar);
		line_iterator_put_line(&itRightVar, variablesData.rightVar);
		return 	investigate_word(line, &itLeftVar, table, dbg_list, flag, line_num, variablesData.leftVar,2) ||
			investigate_word(line, &itRightVar, table, dbg_list, flag, line_num, variablesData.rightVar, 2);

	case 3: /*groups 5, labe, left var and right var*/
		line_iterator_put_line(&itLeftVar, variablesData.leftVar);
		line_iterator_put_line(&itRightVar, variablesData.rightVar);
		line_iterator_put_line(&itLabel, variablesData.label);

		return 	investigate_word(line, &itLeftVar, table, dbg_list, flag, line_num, variablesData.leftVar,3) ||
			investigate_word(line, &itRightVar, table, dbg_list, flag, line_num, variablesData.rightVar, 3) ||
			investigate_word(line, &itLabel, table, dbg_list, flag, line_num, variablesData.label, 3);
	default:
		return;
	}

	
}

bool investigate_word(LineIterator* originalLine,LineIterator* wordIterator, SymbolTable* table, debugList* dbg_list, bool* flag, long line_num, char* wordToInvestigate, int amountOfVars) {
	if (is_register_name_whole(wordIterator)) return FALSE;
	line_iterator_backwards(wordIterator);
	if ((line_iterator_peek(wordIterator) == HASH_CHAR)) return FALSE;
	else {
		if (symbol_table_search_symbol_bool(table, wordToInvestigate)) {
			return TRUE;
		}
		else {
			find_word_start_point(originalLine, wordToInvestigate, amountOfVars);
			debug_list_register_node(dbg_list, debug_list_new_node(originalLine->start, originalLine->current, line_num, ERROR_CODE_LABEL_DOES_NOT_EXISTS));
			(*flag) = TRUE;
			return FALSE;
		}
	}
}

/*type 1 - one var, type*/
void find_word_start_point(LineIterator* it, char* word, int amountOfVars) {
	bool found = FALSE;
	it->current = it->start;
	line_iterator_jump_to(it, COLON_CHAR);
	line_iterator_consume_blanks(it);
	line_iterator_jump_to(it, SPACE_CHAR);

	switch (amountOfVars)
	{
	case 1:
		while(line_iterator_peek(it) != *word){
			line_iterator_advance(it);
		}
		return;
	case 2:
		if(strcmp(line_iterator_next_word(it, ", "),word) == 0) {
			line_iterator_unget_word(it, word);
			line_iterator_consume_blanks(it);
		}
		else {
			line_iterator_jump_to(it,COMMA_CHAR);
		}
		return;
	case 3:
		if (strcmp(line_iterator_next_word(it, "( "), word) == 0) {
			line_iterator_unget_word(it, word);
			line_iterator_consume_blanks(it);
		}
		else if(strcmp(line_iterator_next_word(it, ", "), word) == 0){
			line_iterator_unget_word(it, word);
			line_iterator_consume_blanks(it);
		}
		else {
			line_iterator_jump_to(it, COMMA_CHAR);
			line_iterator_advance(it);
		}
		return;
	default:
		return;
	}
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
* Update the address of a symbol. This is done by looking for labels that are in the symbol table and if found we'll add them to the symbol table
* 
* @param it - Line iterator to the symbol
* @param memory - Pointer to the memory buffer that holds the symbol
* @param table - Pointer to the symbol table that is being updated
*/
void update_symbol_address(LineIterator it, memoryBuffer* memory, SymbolTable* table)
{
	char* line = (char*)xcalloc(strlen(it.start) + 1, sizeof(char));
	LineIterator cpyIt;
	int offset = 0;

	strcpy(line, it.start);
	line_iterator_put_line(&cpyIt, line);
	line_iterator_replace(&cpyIt, "(), ", SPACE_CHAR);
	line_iterator_jump_to(&cpyIt, COLON_CHAR);

	for (char* word = line_iterator_next_word(&cpyIt, " "); word != NULL; word = line_iterator_next_word(&cpyIt, " "), offset++) {
		update_symbol_offset(word, offset, memory, table);
		free(word);
	}

	free(line);
}

void update_symbol_offset(char* word, int offset, memoryBuffer* memory, SymbolTable* table)
{
	LineIterator tmp;
	line_iterator_put_line(&tmp, word);

	if (is_label_name(&tmp)) {
		SymbolTableNode* head = table->head;

		while (head) {
			if (strcmp(head->sym.name, word) == 0 && head->sym.type == SYM_EXTERN) {
				if (head->sym.counter == 0) {
					head->sym.counter = memory->instruction_image.counter + offset;
				}
				else {
					symbol_table_insert_symbol(table, symbol_table_new_node(word, SYM_EXTERN, memory->instruction_image.counter + offset));
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
}

void add_label_base_address(SymbolTable* table)
{
	LIST_FOR_EACH(SymbolTableNode, table->head, head) {
		if (head->sym.type == SYM_DATA || head->sym.type == SYM_CODE)
			head->sym.counter += DECIMAL_ADDRESS_BASE;
	}
}