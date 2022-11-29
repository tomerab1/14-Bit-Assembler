#include "second_pass.h"

bool initiate_second_pass(char* path, SymbolTable* table, int* DC, int* L) {
	FILE* in = open_file(path, MODE_READ);
	LinesListNode lines;
	flags phaseFlags;
	programFinalStatus finalStatus;
	errorContext* error;
	char* curLine = NULL;
	int curLineIndex = 1, IC = 0;

	while ((line = get_line(in)) != NULL) {
		LineIterator curLine;
		line_iterator_put_line(&curLine, line);
		skip_label(curLine);


	}
	finalStatus.createdObject = generate_object_file(lines, path, IC, DC);
	finalStatus.createdExternals = generate_externals_file(lines, table, path);
	finalStatus.createdEntry = generate_entries_file(lines, table,path);
}

bool generate_object_file(LinesListNode* data, char* path, int orders_length, int data_length, errorContext* err) {
	char* outfileName = NULL;
	FILE* out = NULL;
	

	translate_to_machine_data(data,err);

	if(err->err_code == ERROR_CODE_OK){
		fputs(("%9d\t%-9d", orders_length, data_length), out);
		fputs("\n", out);

		while (data != NULL) {
			fputs(("%4d\t%14d", data->address, data->machine_data),out);
			fputs("\n", out);
		}

		outfileName = get_outfile_name(path, ".object");
		out = open_file(out_name, MODE_WRITE);
		
		free(outfileName);
		fclose(out);
	}
	else {
		debug_print_error(err);
	}
}

void translate_to_machine_data(LinesListNode* data, errorContext err) {

}

bool generate_externals_file(LinesListNode* data, SymbolTable* table, char* path){
	char* outfileName = NULL;
	FILE* out = NULL;

	outfileName = get_outfile_name(path, ".external");
	out = open_file(out_name, MODE_WRITE);

	free(outfileName);
	fclose(out);
}

bool generate_entries_file(LinesListNode* data, SymbolTable* table, char* path) {
	char* outfileName = NULL;
	FILE* out = NULL;

	outfileName = get_outfile_name(path, ".entry");
	out = open_file(out_name, MODE_WRITE);
	
	free(outfileName);
	fclose(out);
}

void skip_label(LineIterator* line) {

}

bool extract_order_type(LineIterator* line, flags* flag) {
	if (order_exists) {
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
		flag->dot_entry = false;
		flag->dot_extern = false;
	}
	return false;
}

void find_command(LineIterator* line) {
	bool error = false;

}

void* handle_dot_data(){

}

void* handle_dot_string(){

}

void* handle_dot_extern(){

}

void* handle_dot_entry(){
	 
}

bool order_exists(LineIterator* line) {
	while (!line_iterator_is_end(line)) {
		if (line_iterator_peek(line) == DOT) {
			extract_order_type(line_iterator_advance(line));
			return true;
		}
		line_iterator_advance(tempLine);
	}
}
/*Searchs if entry exists, used later on while generating files*/
void entry_exists(flags* flag){
	flag->dot_entry = true;
}

/*Searchs if extern exists, used later on while generating files*/
void extern_exists(flags* flag){
	flag->dot_extern = true;
}



/*Error handling process*/
bool handle_errors(errorContext* error) {

	return true;
}

/*Finds symbols in table*/
int find_symbol_in_table(char* symbol) {

}

/*Convert decimal to binary*/
void convert_to_binary(char* data){

}

/*Converts binary to decimal*/
void convert_to_deciaml(char* data){

}


