#include <stdio.h>
#include "pre_assembler.h"
#include "memory.h"
#include "debug.h"

int main(int argc, char** argv)
{
//	start_pre_assembler("SOURCE_FILE_TEST.TXT");
	
	//memoryBuffer img = get_new_memory_buffer();

	const char* line = "for (int i = 0; i < 20; i++)";

	debugList* list = debug_list_new_list();
	debugNode* node = debug_list_new_node(line, line + 11, 0, ERROR_CODE_UNKNOWN);

	debug_list_register_node(list, node);

	debug_list_pretty_print(list);

	debug_list_destroy(&list);


	return 0;
}