/** @file
*/

#include "second_pass.h"
#include "constants.h"

//error context appearances are temp 
bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory) {
	FILE* in = open_file(path, MODE_READ);

	programFinalStatus finalStatus = { NULL };
	LineIterator curLine;

	LineIterator* ptrCurLine = &curLine;
	char* line = NULL;
	
	memory->instruction_image.counter = 0; /*init IC counter*/
	while ((line = get_line(in)) != NULL) {
		bool labelFlag = FALSE; /*is current line first word is label*/
		line_iterator_put_line(&curLine, line);
		skip_label(&curLine, &labelFlag, table, &(finalStatus.errors));

		if (!directive_exists(&curLine)) { /*checks if any kind of instruction exists (.something)*/
			execute_line(&curLine, memory);
			memory->instruction_image.counter++;
		}
		else {
			extract_directive_type(line, &finalStatus.entryAndExternFlag);
		}
	}
	/*finished reading all lines in file*/
	if (finalStatus.error_flag) 
		handle_errors(&(finalStatus.errors));
	else 
		create_files(memory, path, &finalStatus, table,  &(finalStatus.errors));

	fclose(in);
	free(line);
	free(&curLine);
}

void execute_line(LineIterator* it, memoryBuffer* memory) {
	char* method = line_iterator_next_word(it, " ");
	int syntaxGroup = get_syntax_group(method);

	execute_command(memory, table,it, method, syntaxGroup);

	free(method);
}

void execute_command(memoryBuffer* memory, SymbolTable* table,LineIterator* restOfLine, char* method, int syntaxGroup) {
	if(line_iterator_includes(restOfLine,))
}

bool generate_object_file(memoryBuffer* memory, char* path, debugList* err) {
	char* outfileName = NULL;
	FILE* out = NULL;
	LinesList* translatedMemory = NULL;
	LinesListNode* lineNode = NULL;

	translatedMemory = translate_to_machine_data(memory,err);
	lineNode = translatedMemory->head;
		if(!lineNode) lineNode = NULL;

		outfileName = get_outfile_name(path, ".object");
		out = open_file(outfileName, MODE_WRITE);

		char placeholder[20];
		sprintf(placeholder, "%9d\t%-9d", memory->data_image.counter, memory->instruction_image.counter);

		fputs(placeholder, out);
		fputs("\n", out);

		while (lineNode != NULL) {
			sprintf(placeholder, "%04d\t%14s", lineNode->address, &lineNode->dataForObject);
			fputs(placeholder,out);
			fputs("\n", out);
		}

	free(outfileName);
	fclose(out);
	
}

LinesList* translate_to_machine_data(memoryBuffer* memory, errorContext* err) {
	int i, j;
	MemoryWord* instImg = memory->instruction_image.memory;
	LinesList* translatedMemory = (LinesList*)xmalloc(sizeof(LinesList) * memory->instruction_image.counter);
	LinesListNode* lineNode = translatedMemory->head;

	for (i = 0; i <= memory->instruction_image.counter; i++) {
		lineNode->address = i;
		unsigned int bits = (instImg[i].mem[1] << 0x08) | (instImg[i].mem[0]);
		for (j = 13; j >= 0; j--) {
			unsigned int mask = 1 << j;
			if ((bits & mask) != 0) {
				lineNode->dataForObject[13 - j] = OBJECT_PRINT_SLASH;
				lineNode->machineData[13 - j] = 1;
			}
			else {
				lineNode->dataForObject[13 - j] = OBJECT_PRINT_DOT;
				lineNode->machineData[13 - j] = 0;
			}
			lineNode = lineNode->next;
		}
		return translatedMemory;
	}

}

bool generate_externals_file(SymbolTable* table, char* path){
	char* outfileName = NULL;
	FILE* out = NULL;
	SymbolTableNode* symTableHead = table->head;

	outfileName = get_outfile_name(path, ".external");
	out = open_file(outfileName, MODE_WRITE);

	char placeholder[20];
	sprintf(placeholder, ("%-10s\t%4d", symTableHead->sym.name, symTableHead->sym.counter));

	while (symTableHead != NULL) {
		if(symTableHead->sym.type == SYM_EXTERN){
		fputs(placeholder, out);
		fputs("\n", out);
		symTableHead->next;
		}
	}

	free(outfileName);
	fclose(out);
}

bool generate_entries_file(SymbolTable* table, char* path) {
	char* outfileName = NULL;
	FILE* out = NULL;
	SymbolTableNode* symTableHead = table->head;

	outfileName = get_outfile_name(path, ".entry");
	out = open_file(outfileName, MODE_WRITE);

	char placeholder[20];
	sprintf(placeholder, ("%-10s\t%4d", ("%-10s%4d", symTableHead->sym.name, symTableHead->sym.counter)));

	while (symTableHead != NULL) {
		if(symTableHead->sym.type == SYM_ENTRY){
			fputs(placeholder, out);
			fputs("\n", out);
			symTableHead->next;
		}
	}

	free(outfileName);
	fclose(out);
}

void create_files(memoryBuffer* memory, char* path, programFinalStatus* finalStatus ,SymbolTable* table,debugList* err) {
	finalStatus->createdObject = generate_object_file(memory, path, &(*finalStatus).errors);
	finalStatus->createdExternals = generate_externals_file(table, path);
	finalStatus->createdEntry = generate_entries_file(table, path);
}

void skip_label(LineIterator* line, bool* labelFlag,SymbolTable* table, debugList* err) {
	if (isLabel(line)) {
		if (symbol_table_search_symbol(table, line_iterator_next_word(line, " "))) { //if exists, needs to edit code so it would care the colon
			line = line->start;
			while (line_iterator_peek(line) != COLON) {
				line_iterator_advance(line);
			}
			line_iterator_advance(line);
			line_iterator_consume_blanks(line);
			labelFlag = TRUE;
		}
		return;
	}
	line->current = line->start;
	return;
}

void extract_directive_type(LineIterator* line, flags* flag) {
		char* command = line_iterator_next_word(line, " ");
		if (strcmp(command, DOT_EXTERN)) {
			extern_exists(flag);
		}
		else if (strcmp(command, DOT_ENTRY)) {
			extern_exists(flag);
		}
		else if(!(strcmp(command, DOT_STRING) || strcmp(command, DOT_DATA))){//isn't any exists command
			debugNode err; //should also add debug list later on function headline
		}
		free(command);
}

bool directive_exists(LineIterator* line) {
	int tempCur = line->current;
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

void extern_exists(flags* flag){
	flag->dot_extern_exists = TRUE;
}

void entry_exists(flags* flag) {
	flag->dot_entry_exists = TRUE;
}

bool handle_errors(debugList* error) {
	
	return TRUE;
}
