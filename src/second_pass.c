#include "second_pass.h"
#include "constants.h"

#include <ctype.h>

struct flags
{
	bool dot_entry_exists;
	bool dot_extern_exists;
};

struct TranslatedMachineData {
	int address;
	char translated[SINGLE_ORDER_SIZE + 1]; /* (+1) for '\0'. */
};

struct programFinalStatus
{
	flags entryAndExternFlag; /*The flags related to the entryand extern directives.*/
		bool createdObject; /* A flag indicating whether an object file was created. */
		bool createdExternals; /* A flag indicating whether an externals file was created. */
		bool createdEntry; /* A flag indicating whether an entry file was created. */
		bool error_flag; /* A flag indicating whether an error occurred during assembly. */
};

bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory)
{
	FILE* in = open_file(path, MODE_READ);
	programFinalStatus finalStatus = { 0 }; /*state manager*/
	LineIterator curLine;
	char* line = NULL;
	long lineNum = 1;

	add_label_base_address(table); /*adds +100 to each label address*/
	img_memory_set_counter(memory_buffer_get_inst_img(memory), 0); /*inits counter*/

	while ((line = get_line(in)) != NULL) { /*Goes over each line*/
		line_iterator_put_line(&curLine, line);
		line_iterator_jump_to(&curLine, COLON_CHAR); /*skips label*/

		if (!directive_exists(&curLine)) { /*checks if any kind of instruction exists (.something)*/
			execute_line(&curLine, table, memory, &finalStatus.error_flag, lineNum); /*exeutes line by verification -> enocding/error throw*/
		}
		else {
			extract_directive_type(&curLine, &finalStatus.entryAndExternFlag); /*in case line is a directive*/
		}

		lineNum++;
		free(line);
	}

	if (finalStatus.error_flag) /*check if any error occured, if so, do not generate new files*/
		return FALSE;

	create_files(memory, path, &finalStatus, table);
	fclose(in);

	return TRUE;
}

void execute_line(LineIterator* it, SymbolTable* table, memoryBuffer* memory, bool* errorFlag, long line_num) {
	if (line_iterator_word_includes(it, DOT_DATA_STRING) || line_iterator_word_includes(it, DOT_STRING_STRING)) {
		return;
	}

	/* Increment counter by one, as every command has a preceding word. */
	img_memory_set_counter(memory_buffer_get_inst_img(memory), img_memory_get_counter(memory_buffer_get_inst_img(memory)) + 1);

	if (is_label_exists_in_line(it, table, errorFlag, line_num)) { /*checks if label exists and valid*/
		update_symbol_address(*it, memory, table); /*updates the address of the symbol*/
		encode_label_start_process(it, memory, table); /*encodes the mem cell according to the label*/
	}
	else {
		skip_first_pass_mem(memory, it); /*if no label exists, skip calculated amount of cells*/
	}
}

void skip_first_pass_mem(memoryBuffer* memory, LineIterator* it) {
	int memCellsToJump = find_amount_of_lines_to_skip(it);
	img_memory_set_counter(memory_buffer_get_inst_img(memory), img_memory_get_counter(memory_buffer_get_inst_img(memory)) + memCellsToJump);
}

int find_amount_of_lines_to_skip(LineIterator* it) {
	char* op = NULL, * operand1 = NULL, * operand2 = NULL, * operand3 = NULL;
	char* tempLine = (char*)xmalloc(sizeof(char) * (strlen(it->start) + 1));
	LineIterator tempIt;
	int total = 0;

	strcpy(tempLine, it->start);
	line_iterator_put_line(&tempIt, tempLine);
	line_iterator_replace(&tempIt, "(), ", SPACE_CHAR);
	line_iterator_jump_to(&tempIt, COLON_CHAR);

	op = line_iterator_next_word(&tempIt, SPACE_STRING);
	operand1 = line_iterator_next_word(&tempIt, SPACE_STRING);
	operand2 = line_iterator_next_word(&tempIt, SPACE_STRING);
	operand3 = line_iterator_next_word(&tempIt, SPACE_STRING);

	/* 1 for the opcode and one for the shared memory word of 2 registers. */
	if ((operand1 && operand2) && (*operand1 == REG_BEG_CHAR && *operand2 == REG_BEG_CHAR))
		total = isdigit(*(operand1 + 1)) && isdigit(*(operand2 + 1)) ? 1 : 2;
	/* 1 for the opcode and one for the shared memory word of 2 registers, and + 1 for the label name (this case is for parametrized labels) */
	else if ((operand2 && operand3) && (*operand2 == REG_BEG_CHAR && *operand3 == REG_BEG_CHAR))
		total = isdigit(*(operand1 + 1)) && isdigit(*(operand2 + 1)) ? 2 : 3;
	else {
		total += (operand1) ? 1 : 0;
		total += (operand2) ? 1 : 0;
		total += (operand3) ? 1 : 0;
	}

	free(op);
	free(operand1);
	free(operand2);
	free(operand3);
	free(tempLine);

	return total; /* 1 for the opcode, 2 for each individual memory word */
}

bool generate_object_file(memoryBuffer* memory, char* path)
{
	char* outfileName = NULL;
	FILE* out = NULL;
	TranslatedMachineData* translatedMemory = NULL;
	char placeholder[50] = { 0 };
	int i;

	translatedMemory = translate_to_machine_data(memory);
	outfileName = get_outfile_name(path, ".object");
	out = open_file(outfileName, MODE_WRITE);

	/* Write the instruction and data image counters to the output file */
	sprintf(placeholder, "%9d\t%4d\n", img_memory_get_counter(memory_buffer_get_inst_img(memory)), img_memory_get_counter(memory_buffer_get_data_img(memory)));
	fputs(placeholder, out);

	/* Iterate over the translated memory and write it to the output file */
	for (i = 0; i < img_memory_get_counter(memory_buffer_get_inst_img(memory)) + img_memory_get_counter(memory_buffer_get_data_img(memory)); i++) {
		sprintf(placeholder, "%04d\t%s\n", DECIMAL_ADDRESS_BASE + translatedMemory[i].address, translatedMemory[i].translated);
		fputs(placeholder, out);
	}

	free(translatedMemory);
	free(outfileName);
	fclose(out);

	return TRUE;
}

TranslatedMachineData* translate_to_machine_data(memoryBuffer* memory)
{
	int i = 0, total = img_memory_get_counter(memory_buffer_get_data_img(memory)) + img_memory_get_counter(memory_buffer_get_inst_img(memory));

	/* Allocate memory for the translated machine code */
	TranslatedMachineData* translatedMemory = (TranslatedMachineData*)xmalloc(total * sizeof(TranslatedMachineData));

	img_memory_get_counter(memory_buffer_get_data_img(memory));

	/* Decode the instruction and data memory buffers and store in the translated memory */
	decode_memory(translatedMemory, memory_buffer_get_inst_img(memory), &i, img_memory_get_counter(memory_buffer_get_inst_img(memory)) + 1);
	decode_memory(translatedMemory, memory_buffer_get_data_img(memory), &i, total);

	return translatedMemory;

}

void decode_memory(TranslatedMachineData* tmd, imageMemory* inst, int* startPos, int endPos)
{
	int i, j, k = *startPos;

	for (i = 0; k < endPos; i++) {
		/* Get the current memory word and set the address of the translated machine data */
		unsigned int bits = (memory_word_get_memory(img_memory_get_memory_at(inst, i))[1] << 0x08) | memory_word_get_memory(img_memory_get_memory_at(inst, i))[0];
		tmd[k].address = k;
		memset(tmd[k].translated, 0, sizeof(tmd[k].translated));

		 /*translated the memory from binary to slashes and dots*/
		for (j = 13; j >= 0; j--) {
			unsigned int mask = 1 << j;
			if ((bits & mask) != 0) {
				tmd[k].translated[13 - j] = OBJECT_PRINT_SLASH;
			}
			else {
				tmd[k].translated[13 - j] = OBJECT_PRINT_DOT;
			}
		}
		k++;
	}
	*startPos = k - 1;
}

bool generate_externals_file(SymbolTable* table, char* path) {
	char* outfileName = NULL;/* Pointer to the filename of the output file */
	FILE* out = NULL;

	SymbolTableNode* symTableHead = symbol_table_get_head(table);
	char placeholder[20] = { 0 }; /* Placeholder string for writing data to the file */

	/* Get the name of the output file */
	outfileName = get_outfile_name(path, EXTERN_ASSEMBLER_FILE_EXTENSTION);
	/* Open the output file for writing */
	out = open_file(outfileName, MODE_WRITE);

	/* Iterate over the symbol table and write the names and addresses of external symbols to the file */
	while (symTableHead != NULL) {
		if (symbol_get_type(symbol_node_get_sym(symTableHead)) == SYM_EXTERN) {
			sprintf(placeholder, "%s\t%d\n", symbol_get_name(symbol_node_get_sym(symTableHead)), symbol_get_counter(symbol_node_get_sym(symTableHead)));
			fputs(placeholder, out);
		}
		symTableHead = symbol_node_get_next(symTableHead);
	}

	/* Free the filename memory and close the output file */
	free(outfileName);
	fclose(out);

	return TRUE; /* Return true to indicate successful file generation */

}

bool generate_entries_file(SymbolTable* table, char* path) {
	char* outfileName = NULL;
	FILE* out = NULL;
	SymbolTableNode* symTableHead = symbol_table_get_head(table);
	char placeholder[20] = { 0 };

	outfileName = get_outfile_name(path, ENTRY_ASSEMBLER_FILE_EXTENSTION);
	out = open_file(outfileName, MODE_WRITE);

	/* iterate over symbol table and write the entries to file */
	while (symTableHead != NULL) {
		if (symbol_get_type(symbol_node_get_sym(symTableHead)) == SYM_ENTRY) {
			sprintf(placeholder, "%s\t%d\n", symbol_get_name(symbol_node_get_sym(symTableHead)), symbol_get_counter(symbol_node_get_sym(symTableHead)));
			fputs(placeholder, out);
		}
		symTableHead = symbol_node_get_next(symTableHead);
	}

	free(outfileName);
	fclose(out);

	return TRUE;
}

void create_files(memoryBuffer* memory, char* path, programFinalStatus* finalStatus, SymbolTable* table)
{
	/*Generate object file and update finalStatus accordingly*/
	finalStatus->createdObject = generate_object_file(memory, path);

	/*If the symbol table has externals, generate external file and update finalStatus accordingly*/
	if (symbol_table_get_hasExternals(table))
		finalStatus->createdExternals = generate_externals_file(table, path);

	/*If the symbol table has entries, generate entry file and update finalStatus accordingly*/
	if (symbol_table_get_hasEntries(table))
		finalStatus->createdEntry = generate_entries_file(table, path);
}

void extract_directive_type(LineIterator* line, flags* flag) {
    char* command = line_iterator_next_word(line, SPACE_STRING);
	/* Checks if command is extern */
    if (strcmp(command, EXTERN_STRING)) {
        extern_exists(flag);
    }
	/* Checks if command is entry */
    else if (strcmp(command, ENTRY_STRING)) {
        extern_exists(flag);
    }

    free(command);
}

VarData* extract_variables(LineIterator* it) {
	VarData* variables = NULL;
	char* opcode = line_iterator_next_word(it, SPACE_STRING);
	SyntaxGroups synGroup = get_syntax_group(opcode);

	/* Extract variables for group 1, 2, and 7 opcodes */
    if (synGroup == SG_GROUP_1 || synGroup == SG_GROUP_2 || synGroup == SG_GROUP_7) {
        variables = extract_variables_group_1_and_2_and_7(it);
    }

	/* Extract variables for group 3 and 6 opcodes */
    else if (synGroup == SG_GROUP_3 || synGroup == SG_GROUP_6) {
        variables = extract_variables_group_3_and_6(it);
    }

	/* Extract variables for group 5 opcodes */
    else if (synGroup == SG_GROUP_5) {
        variables = extract_variables_group_5(it);
    }

	free(opcode);

	return variables;
}

bool is_label_exists_in_line(LineIterator* line, SymbolTable* table, bool* flag, long line_num) {
	VarData* variablesData = NULL;
	bool ret_val = TRUE;
	LineIterator itLeftVar, itRightVar, itLabel;

	variablesData = extract_variables(line);

	if (!variablesData) {
		return TRUE;
	}

	/*Cases based on amount of variables*/
	switch (varData_get_total(variablesData))
	{
	case 1: /*group 3 and 6, left var*/
		if (varData_get_leftVar(variablesData) != NULL) {
			line_iterator_put_line(&itLeftVar, varData_get_leftVar(variablesData));
			ret_val = investigate_word(line, &itLeftVar, table, flag, line_num, varData_get_leftVar(variablesData), 1);
		}
		else {
			line_iterator_put_line(&itLabel, varData_get_label(variablesData));
			ret_val = investigate_word(line, &itLabel, table, flag, line_num, varData_get_label(variablesData), 1);
		}
		break;

	/* We use a bitwise OR operator because logical OR will not propogate to the next check if one of the results is true. */
	case 2: /*groups 1,2,7, left var and right var*/

		line_iterator_put_line(&itLeftVar, varData_get_leftVar(variablesData));
		line_iterator_put_line(&itRightVar, varData_get_rightVar(variablesData));
		ret_val = investigate_word(line, &itLeftVar, table, flag, line_num, varData_get_leftVar(variablesData), 2) |
				  investigate_word(line, &itRightVar, table, flag, line_num, varData_get_rightVar(variablesData), 2);
		break;

	case 3: /*groups 5, labe, left var and right var*/
		line_iterator_put_line(&itLeftVar, varData_get_leftVar(variablesData));
		line_iterator_put_line(&itRightVar, varData_get_rightVar(variablesData));
		line_iterator_put_line(&itLabel, varData_get_label(variablesData));

		ret_val = investigate_word(line, &itLeftVar, table, flag, line_num, varData_get_leftVar(variablesData), 3) |
				  investigate_word(line, &itRightVar, table, flag, line_num, varData_get_rightVar(variablesData), 3) |
				  investigate_word(line, &itLabel, table, flag, line_num, varData_get_label(variablesData), 3);
		break;
	}

	varData_destroy(&variablesData);

	return ret_val;
}

bool investigate_word(LineIterator* originalLine, LineIterator* wordIterator, SymbolTable* table, bool* flag, long line_num, char* wordToInvestigate, int amountOfVars) {
	if (is_register_name_whole(wordIterator)) /*If the word is a register name, return FALSE*/
		return FALSE;

	line_iterator_backwards(wordIterator);

	if ((line_iterator_peek(wordIterator) == HASH_CHAR))
		return FALSE;
	else {
		if (symbol_table_search_symbol_bool(table, wordToInvestigate)) { /*If the word exists in the symbol table, return TRUE*/
			return TRUE;
		}
		else { /*If the word doesn't exist in the symbol table*/
			find_word_start_point(originalLine, wordToInvestigate, amountOfVars);
			print_error(originalLine->start, line_num, ERROR_CODE_LABEL_DOES_NOT_EXISTS);
			(*flag) = TRUE; /*sets errors flag to true*/
			return FALSE;
		}
	}
}

void find_word_start_point(LineIterator* it, char* word, int amountOfVars) {
	line_iterator_reset(it);
	line_iterator_jump_to(it, COLON_CHAR);
	line_iterator_consume_blanks(it);
	line_iterator_jump_to(it, SPACE_CHAR);

	switch (amountOfVars)
	{
	case 1:
		while (line_iterator_peek(it) != *word) {
			line_iterator_advance(it);
		}
		break;
	case 2:
		if (strcmp(line_iterator_next_word(it, COMMA_STRING), word) == 0) {
			line_iterator_unget_word(it, word);
			line_iterator_consume_blanks(it);
		}
		else {
			line_iterator_jump_to(it, COMMA_CHAR);
		}
		break;
	case 3:
		if (strcmp(line_iterator_next_word(it,OPEN_PAREN_STRING_W_SPACE), word) == 0) {
			line_iterator_unget_word(it, word);
			line_iterator_consume_blanks(it);
		}
		else if (strcmp(line_iterator_next_word(it, COMMA_STRING), word) == 0) {
			line_iterator_unget_word(it, word);
			line_iterator_consume_blanks(it);
		}
		else {
			line_iterator_jump_to(it, COMMA_CHAR);
			line_iterator_advance(it);
		}
		break;
	default:
		break;
	}
}

void extern_exists(flags* flag) {
	flag->dot_extern_exists = TRUE;
}

void entry_exists(flags* flag) {
	flag->dot_entry_exists = TRUE;
}

void update_symbol_address(LineIterator it, memoryBuffer* memory, SymbolTable* table)
{
	char* line = (char*)xcalloc(strlen(it.start) + 1, sizeof(char));
	char* word = NULL;
	LineIterator cpyIt;
	int offset = 0;

	/* Copies the line and replaces some characters to spaces to simplify parsing. */
	strcpy(line, it.start);
	line_iterator_put_line(&cpyIt, line);
	line_iterator_replace(&cpyIt, "(), ", SPACE_CHAR);
	line_iterator_jump_to(&cpyIt, COLON_CHAR);

	/* Iterates through the words in the line, updating the offset of each symbol found. */
	for (word = line_iterator_next_word(&cpyIt, SPACE_STRING); word != NULL; word = line_iterator_next_word(&cpyIt, SPACE_STRING), offset++) {
		update_symbol_offset(word, offset, memory, table);
		free(word);
	}

	free(line);
}

void update_symbol_offset(char* word, int offset, memoryBuffer* memory, SymbolTable* table)
{
	LineIterator tmp;
	line_iterator_put_line(&tmp, word);

	if (is_label_name(&tmp)) { 	/* Searches for the symbol in the symbol table and updates its counter based on its type. */
		SymbolTableNode* head = symbol_table_get_head(table);

		while (head) {
			if (strcmp(symbol_get_name(symbol_node_get_sym(head)), word) == 0 && symbol_get_type(symbol_node_get_sym(head)) == SYM_EXTERN) { /*finds the symbols of type extern*/
				if (symbol_get_counter(symbol_node_get_sym(head)) == 0) {
					symbol_set_counter(symbol_node_get_sym(head), DECIMAL_ADDRESS_BASE + img_memory_get_counter(memory_buffer_get_inst_img(memory)) + offset - 1); /*updates offset*/
				}
				else {
					symbol_table_insert_symbol(table, symbol_table_new_node(word, SYM_EXTERN, DECIMAL_ADDRESS_BASE + img_memory_get_counter(memory_buffer_get_inst_img(memory)) + offset - 1));/*updates offset*/
					break;
				}
			}
			else if (strcmp(symbol_get_name(symbol_node_get_sym(head)), word) == 0 && symbol_get_type(symbol_node_get_sym(head)) == SYM_ENTRY) { /*finds the symbols of type entry*/
				SymbolTableNode* defHead = symbol_table_get_head(table);

				while (defHead) {
					if (strcmp(symbol_get_name(symbol_node_get_sym(defHead)), word) == 0 && (symbol_get_type(symbol_node_get_sym(defHead)) == SYM_CODE || symbol_get_type(symbol_node_get_sym(defHead)) == SYM_DATA)) {
						symbol_set_counter(symbol_node_get_sym(head), symbol_get_counter(symbol_node_get_sym(defHead)));
						break;
					}

					defHead = symbol_node_get_next(defHead);
				}
			}

			head = symbol_node_get_next(head);
		}
	}
}

void add_label_base_address(SymbolTable* table)
{
	SymbolTableNode* head = symbol_table_get_head(table);

	while (head) {
		if (symbol_get_type(symbol_node_get_sym(head)) == SYM_DATA || symbol_get_type(symbol_node_get_sym(head)) == SYM_CODE) {
			symbol_set_counter(symbol_node_get_sym(head), symbol_get_counter(symbol_node_get_sym(head)) + DECIMAL_ADDRESS_BASE); /*updates offset*/
		}
		head = symbol_node_get_next(head);
	}
}
