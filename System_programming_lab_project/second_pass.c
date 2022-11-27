#include "second_pass.h"
bool initiate_second_pass(char* path) {
}

bool generate_object_file(FILE* out, char* data, int orders_length, int data_length){

}

bool generate_externals_file(FILE* out, void* data, bool isExists){

}

bool generate_entries_file(FILE* out, void* data, bool isExists){

}

int extract_order_type(char* line){

}

void* handle_dot_data(){

}

void* handle_dot_string(){

}

void* handle_dot_extern(){

}


void* handle_dot_entry(){
	 
}

//Searchs if entry exists, used later on while generating files
bool entry_exists(){

}

//Searchs if extern exists, used later on while generating files
bool extern_exists(){

}

//Error handling process
void handle_errors(error err){

}

//Finds symbols in table
int find_symbol_in_table(char* symbol){

}

//Convert decimal to binary
void convert_to_binary(char* data){

}

//Converts binary to decimal
void convert_to_deciaml(char* data){

}