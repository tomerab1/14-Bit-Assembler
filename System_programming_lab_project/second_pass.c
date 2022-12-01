#include "second_pass.h"
//error context appearances are temp 
bool initiate_second_pass(char* path, SymbolTable* table, memoryBuffer* memory) {
	FILE* in = open_file(path, MODE_READ);

	programFinalStatus finalStatus;
	errorContext* error = NULL;
	flags phaseFlags;
	
	int curLineIndex = 1;
	char* line = NULL;

	while ((line = get_line(in)) != NULL) {
		LineIterator curLine;
		LineIterator* ptrCurLine;
		bool labelFlag = FALSE;
		line_iterator_put_line(ptrCurLine, line);
		skip_label(ptrCurLine, &labelFlag, table, error);
		extract_order_type(ptrCurLine, &phaseFlags);
		//stopped here

	}

	finalStatus.createdObject = generate_object_file(memory, path, error);
	finalStatus.createdExternals = generate_externals_file(table, path);
	finalStatus.createdEntry = generate_entries_file(table,path);

	fclose(in);
	free(line);
	free(error);
}

bool generate_object_file(memoryBuffer* memory, char* path, errorContext* err) {
	char* outfileName = NULL;
	FILE* out = NULL;
	LinesList* translatedMemory;
	LinesListNode* lineNode;

	translatedMemory = translate_to_machine_data(memory,err);
	lineNode = translatedMemory->head;

	if(err->err_code == ERROR_CODE_OK){
		outfileName = get_outfile_name(path, ".object");
		out = open_file(outfileName, MODE_WRITE);

		char placeholder[20];
		sprintf(placeholder, ("%9d\t%-9d", memory->data_image.counter, memory->instruction_image.counter));

		fputs(placeholder, out);
		fputs("\n", out);

		while (lineNode != NULL) {
			sprintf(placeholder, ("%04d\t%14d", lineNode->address, lineNode->machine_data));
			fputs(placeholder,out);
			fputs("\n", out);
		}
	}
	else {
		/*debug_print_error(err);*/
	}
	free(outfileName);
	fclose(out);
}

LinesList* translate_to_machine_data(memoryBuffer* memory, errorContext* err) {
	int i,j;
	LinesList* translatedMemory = (LinesList*)xmalloc(sizeof(LinesList)*memory->instruction_image.counter);
	LinesListNode* lineNode = translatedMemory->head;
	for (i = 0; i <= memory->instruction_image.counter; i++) {
		int countSize = 0;
		for (j = 0; j < PARAM_ONE_SIZE; j++) {
			transform_binary(memory->data_image.memory[i].param2, lineNode->machine_data, j, countSize++);
			countSize++;
		}
		for (j; j < PARAM_TWO_SIZE; j++) {
			transform_binary(memory->data_image.memory[i].param2, lineNode->machine_data, j, countSize++);
			countSize++;
		}
		for (j; j < OPCODE_SIZE; j++) {
			transform_binary(memory->data_image.memory[i].param2, lineNode->machine_data, j, countSize++);
			countSize++;
		}
		for (j; j < DEST_SIZE; j++) {
			transform_binary(memory->data_image.memory[i].param2, lineNode->machine_data, j, countSize++);
			countSize++;
		}
		for (j; j < SOURCE_SIZE; j++) {
			transform_binary(memory->data_image.memory[i].param2, lineNode->machine_data, j, countSize++);
			countSize++;
		}
		for (j; j < E_R_A_SIZE; j++) {
			transform_binary(memory->data_image.memory[i].param2, lineNode->machine_data, j, countSize++);
			countSize++;
		}
		
	}
	return translatedMemory;
}

void transform_binary(char* data,char* machineCodeString, int currentIndexData, int currentIndexMCS) {
	if (*(data + currentIndexData) == 0) {
		*(machineCodeString + currentIndexMCS) = OBJECT_PRINT_DOT;
		return;
	}
	*(machineCodeString + currentIndexMCS) = OBJECT_PRINT_SLASH;
}

bool generate_externals_file(SymbolTable* table, char* path){
	char* outfileName = NULL;
	FILE* out = NULL;
	SymbolTableNode* symTableHead = table->head;

	outfileName = get_outfile_name(path, ".external");
	out = open_file(outfileName, MODE_WRITE);

	while (symTableHead != NULL && symTableHead->sym.type == SYM_EXTERN) {
		fputs(("%-10s\t%4d", symTableHead->sym.name, symTableHead->sym.counter), out);
		fputs("\n", out);
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

	while (symTableHead != NULL && symTableHead->sym.type == SYM_ENTRY) {
		fputs(("%-10s%4d", symTableHead->sym.name, symTableHead->sym.counter), out);
		fputs("\n", out);
	}

	free(outfileName);
	fclose(out);
}

void skip_label(LineIterator* line, bool* labelFlag,SymbolTable* table, errorContext* err) {
	if (isLabel(line)) {
			if (symbol_table_search_symbol(table, line_iterator_next_word(line))) { //if exists, needs to edit code so it would care the colon
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
	err = TRUE; //TEMP
	return;
}

bool isLabel(LineIterator* line) {
	LineIterator* tempLineIterator;
	line_iterator_put_line(tempLineIterator, line_iterator_next_word(line));
	line_iterator_backwards(tempLineIterator);
	line->current = line->start;

	if (line_iterator_peek(tempLineIterator) == COLON){
		free(tempLineIterator);
		return TRUE;
	}

	free(tempLineIterator);
	return FALSE;
}

bool extract_order_type(LineIterator* line, flags* flag) {
	if (order_exists(line,flag)) {
		char* command = line_iterator_next_word(line);
		if (strcmp(command, DOT_DATA)) {
			handle_dot_data();
		}
		else if (strcmp(command, DOT_STRING)) {
			handle_dot_string();
		}
		else if (strcmp(command, DOT_EXTERN)) {
			handle_dot_extern();
		}
		else if (strcmp(command, DOT_ENTRY)) {
			handle_dot_entry();
		}
		else {
			errorContext err;
			return handle_errors(&err);
		}
		free(command);
	}
	else {
		flag->dot_entry = FALSE;
		flag->dot_extern = FALSE;
	}
	return FALSE;
}

void find_command(LineIterator* line) {

}

void* handle_dot_data(){

}

void* handle_dot_string(){

}

void* handle_dot_extern(){

}

void* handle_dot_entry(){
	 
}

bool order_exists(LineIterator* line, flags* flag) {
	while (!line_iterator_is_end(line)) {
		if (line_iterator_peek(line) == DOT_COMMAND) {
			line_iterator_advance(line);
			extract_order_type(line,flag);
			printf("");
			return TRUE;
		}
		line_iterator_advance(line);
	}
}

/*Searchs if extern exists, used later on while generating files*/
bool extern_exists(flags* flag){
	flag->dot_extern = TRUE;
}
/*Searchs if extern exists, used later on while generating files*/
bool entry_exists(flags* flag) {
	flag->dot_entry = TRUE;
}

/*Error handling process*/
bool handle_errors(errorContext* error) {

	return TRUE;
}

/*Convert decimal to binary*/
void convert_to_binary(char* data){

}

/*Converts binary to decimal*/
void convert_to_deciaml(char* data){

}


