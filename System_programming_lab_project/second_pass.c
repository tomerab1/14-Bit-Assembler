#include "second_pass.h"

bool initiate_second_pass(char* path) {

}

bool generate_object_file(FILE* out, char* data, int orders_length, int data_length) {

}

bool generate_externals_file(FILE* out, void* data, bool isExists) {

}

bool generate_entries_file(FILE* out, void* data, bool isExists) {

}


void skip_label(LineIterator* line) {

}

int extract_order_type(LineIterator* line, flags* flag) {
	if (order_exists) {

	}
	else {
		flag->dot_entry = false;
		flag->dot_extern = false;
	}
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
void handle_errors(error err) {

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


