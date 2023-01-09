/** @file
*/

#include "second_pass.h"
#include "constants.h"

//error context appearances are temp 
bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory) {
	FILE* in = open_file(path, MODE_READ);

	programFinalStatus finalStatus = { NULL };
	LineIterator curLine = { 0 };
	char* line = NULL;

	memory->instruction_image.counter = 0; /*init IC counter*/
	while ((line = get_line(in)) != NULL) {
		bool labelFlag = FALSE; /*is current line first word is label*/
		line_iterator_put_line(&curLine, line);
		line_iterator_jump_to(&curLine, COLON);

		if (!directive_exists(&curLine)) { /*checks if any kind of instruction exists (.something)*/
			execute_line(&curLine, table, memory);
		}
		else {
			extract_directive_type(&curLine, &finalStatus.entryAndExternFlag);
		}
	}

	/*finished reading all lines in file*/
	if (finalStatus.error_flag) {
		handle_errors(&(finalStatus.errors));
		return FALSE;
	}
	else if (TRUE) {
		create_files(memory, path, &finalStatus, table, &(finalStatus.errors));
	}

	fclose(in);
	free(line);
	free(&curLine);
	return TRUE;
}

void execute_line(LineIterator* it, SymbolTable* table, memoryBuffer* memory) {
	if (is_label_exists_in_line((*it), (*table))) {
		encode_label_start_process(it, memory, table);
	}
	else {
		skip_first_pass_mem(memory,it);
	}
}


void skip_first_pass_mem(memoryBuffer* memory,LineIterator* it) {
	int memCellsToJump = find_amount_of_lines_to_skip(it);
	memory->instruction_image.counter += memCellsToJump;
}

int find_amount_of_lines_to_skip(LineIterator* it) {
	char* op = line_iterator_next_word(it, SPACE_CHAR);
	SyntaxGroups opGroup = get_syntax_group(op);
	VarData variables = { 0 };
	int totalJumps = 1;

	if (opGroup == 1 || opGroup == 2 || opGroup == 7) {
		variables = extract_variables_group_1_and_2_and_7(it);
		if (get_operand_kind(variables.leftVar) == KIND_REG && get_operand_kind(variables.rightVar) == KIND_REG) {
			return totalJumps + 1;
		}
		else {
			return totalJumps + 2;
		}
	}
	else if (opGroup == 3 || opGroup == 6){
		return totalJumps + 1;
	}
	else if (opGroup == 5){
		variables = extract_variables_group_5(it);
		if (get_operand_kind(variables.leftVar) == KIND_REG && get_operand_kind(variables.rightVar) == KIND_REG) {
			return totalJumps + 2;
		}
		return variables.total + totalJumps;
	}
	else
	{
		/*throw error here*/
		return -1;
	}
}

bool generate_object_file(memoryBuffer* memory, char* path, debugList* err) {
	char* outfileName = NULL;
	FILE* out = NULL;
	LinesListNode* linesNode = NULL;
	char placeholder[20];
	bool completed = FALSE;

	linesNode = translate_to_machine_data(memory, err);
	outfileName = get_outfile_name(path, ".object");
	out = open_file(outfileName, MODE_WRITE);

	sprintf(placeholder, "%9d\t%-9d\n", memory->data_image.counter, memory->instruction_image.counter);
	fputs(placeholder, out);

	while (!completed) {
		sprintf(placeholder, "%04d\t%14s\n", linesNode->address, linesNode->dataForObject);
		fputs(placeholder, out);

		if (linesNode->address == memory->instruction_image.counter)
			completed = TRUE;
	}

	free(outfileName);
	fclose(out);
	return TRUE;
}

LinesListNode* translate_to_machine_data(memoryBuffer* memory, debugList* err) {
	int i, j;
	MemoryWord* instImg = memory->instruction_image.memory;	
	LinesListNode* translatedMemory = (LinesListNode*)xmalloc(sizeof(LinesListNode) * memory->instruction_image.counter);
	
	for (i = 0; i < memory->instruction_image.counter; i++) {
		unsigned int bits = (instImg[i].mem[1] << 0x08) | (instImg[i].mem[0]);
		translatedMemory[i].address = 100+i;
		
		for (j = 13; j >= 0; j--) {
			unsigned int mask = 1 << j;
			if ((bits & mask) != 0) {
				translatedMemory[i].dataForObject[13 - j] = OBJECT_PRINT_SLASH;
			}
			else {
				translatedMemory[i].dataForObject[13 - j] = OBJECT_PRINT_DOT;
			}
		}
	}

	return translatedMemory;
}

bool generate_externals_file(SymbolTable* table, char* path) {
	char* outfileName = NULL;
	FILE* out = NULL;
	SymbolTableNode* symTableHead = table->head;
	char placeholder[20];

	outfileName = get_outfile_name(path, ".external");
	out = open_file(outfileName, MODE_WRITE);


	while (symTableHead != NULL) {
		if (symTableHead->sym.type == SYM_EXTERN) {
			sprintf(placeholder, "%s\t%d\n", symTableHead->sym.name, symTableHead->sym.counter);
			fputs(placeholder, out);
		}
		symTableHead->next;
	}

	free(outfileName);
	fclose(out);
	return TRUE;
}

bool generate_entries_file(SymbolTable* table, char* path) {
	char* outfileName = NULL;
	FILE* out = NULL;
	SymbolTableNode* symTableHead = table->head;
	char placeholder[20];

	outfileName = get_outfile_name(path, ".entry");
	out = open_file(outfileName, MODE_WRITE);

	while (symTableHead != NULL) {
		if (symTableHead->sym.type == SYM_EXTERN) {
			sprintf(placeholder, "%s\t%d\n", symTableHead->sym.name, symTableHead->sym.counter);
			fputs(placeholder, out);
		}
		symTableHead->next;
	}

	free(outfileName);
	fclose(out);
	return TRUE;
}

void create_files(memoryBuffer* memory, char* path, programFinalStatus* finalStatus, SymbolTable* table, debugList* err) {
	finalStatus->createdObject = generate_object_file(memory, path, &(*finalStatus).errors);
	finalStatus->createdExternals = generate_externals_file(table, path);
	finalStatus->createdEntry = generate_entries_file(table, path);
}

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

bool directive_exists(LineIterator* line) {
	char* tempCur = line->current;
	while (!line_iterator_is_end(line)) {
		if (line_iterator_peek(line) == DOT_COMMAND) {
			line_iterator_advance(line);
			return TRUE; /*in case does exists, return afterward*/
		}

		line_iterator_advance(line);
	}

	line->current = tempCur;
	return FALSE;
}

void extern_exists(flags* flag) {
	flag->dot_extern_exists = TRUE;
}

void entry_exists(flags* flag) {
	flag->dot_entry_exists = TRUE;
}

bool handle_errors(debugList* error)
{
	return TRUE;
}