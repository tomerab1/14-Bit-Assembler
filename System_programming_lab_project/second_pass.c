#include "second_pass.h"

bool initiate_second_pass(char* path, SymbolTable* table, int* DC, int* L) {
	FILE* in = open_file(path, MODE_READ);

	programFinalStatus finalStatus;
	LinesListNode lines;
	errorContext* error = NULL;
	flags phaseFlags;
	
	int curLineIndex = 1, IC = 0;
	char* line = NULL;

	while ((line = get_line(in)) != NULL) {
		LineIterator curLine;
		line_iterator_put_line(&curLine, line);
		skip_label(&curLine);


	}
	finalStatus.createdObject = generate_object_file(&lines, path, IC, (*DC),error);
	finalStatus.createdExternals = generate_externals_file(&lines, table, path);
	finalStatus.createdEntry = generate_entries_file(&lines, table,path);

	fclose(in);
	free(line);
	free(error);
}

bool generate_object_file(LinesListNode* data, char* path, int orders_length, int data_length, errorContext* err) {
	char* outfileName = NULL;
	FILE* out = NULL;

	translate_to_machine_data(data,*err);

	if(err->err_code == ERROR_CODE_OK){
		outfileName = get_outfile_name(path, ".object");
		out = open_file(outfileName, MODE_WRITE);

		fputs(("%9d\t%-9d", orders_length, data_length), out);
		fputs("\n", out);

		while (data != NULL) {
			fputs(("%04d\t%14d", data->address, data->machine_data),out);
			fputs("\n", out);
		}
	}
	else {
		/*debug_print_error(err);*/
	}
	free(outfileName);
	fclose(out);
}

void translate_to_machine_data(LinesListNode* data, errorContext err) {

}

bool generate_externals_file(LinesListNode* data, SymbolTable* table, char* path){
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

bool generate_entries_file(LinesListNode* data, SymbolTable* table, char* path) {
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

void skip_label(LineIterator* line) {

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


