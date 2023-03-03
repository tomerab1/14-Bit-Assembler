/** @file
*/

#include "second_pass.h"
#include "constants.h"

//error context appearances are temp 
bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory, debugList* dbg_list)
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

void execute_line(LineIterator* it, SymbolTable* table, memoryBuffer* memory, debugList* dbg_list, bool* errorFlag,long line_num) {
	/* Increment counter by one, as every command has a preceding word. */
	memory->instruction_image.counter++;
	if (is_label_exists_in_line(it, table,dbg_list, errorFlag, line_num)) 
		encode_label_start_process(it, memory, table, dbg_list);
	else 
		skip_first_pass_mem(memory, it);
}

void skip_first_pass_mem(memoryBuffer* memory, LineIterator* it) {
	int memCellsToJump = find_amount_of_lines_to_skip(it);
	memory->instruction_image.counter += memCellsToJump;
}

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
				translatedMemory[i].translated[j] = OBJECT_PRINT_SLASH;
			}
			else {
				translatedMemory[i].translated[j] = OBJECT_PRINT_DOT;
			}
		}
	}

	return translatedMemory;
}

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

void create_files(memoryBuffer* memory, char* path, programFinalStatus* finalStatus, SymbolTable* table)
{
	finalStatus->createdObject = generate_object_file(memory, path);
	table->hasExternals ? finalStatus->createdExternals = generate_externals_file(table, path) : NULL;
	table->hasEntries ? finalStatus->createdEntry = generate_entries_file(table, path) : NULL;
}

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

bool investigate_word(LineIterator* originalLine,LineIterator* wordIterator, SymbolTable* table, debugList* dbg_list, bool* flag, long line_num, char* wordToInvestigate,int amountOfVars) {
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

bool directive_exists(LineIterator* line) {
	return line_iterator_word_includes(line, ".string") ||
		   line_iterator_word_includes(line, ".data") ||
		   line_iterator_word_includes(line, ".extern") ||
		   line_iterator_word_includes(line, ".entry");
}

void extern_exists(flags* flag) {
	flag->dot_extern_exists = TRUE;
}

void entry_exists(flags* flag) {
	flag->dot_entry_exists = TRUE;
}