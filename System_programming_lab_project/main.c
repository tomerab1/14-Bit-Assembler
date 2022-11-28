#include <stdio.h>
#include "pre_assembler.h"
#include "memory.h"
#include "debug.h"
#include "first_pass.h"

int main(int argc, char** argv)
{
	start_pre_assembler("SOURCE_FILE_TEST.TXT");
	
	const char* pre_assembled_path = get_outfile_name("SOURCE_FILE_TEST", ".am");
	memoryBuffer img = memory_buffer_get_new();
	SymbolTable* sym_table = symbol_table_new_table();
	debugList* dbg_list = debug_list_new_list();

	if (do_first_pass(pre_assembled_path, &img, sym_table, dbg_list)) {
		/* Create all the appropriate files, continue to second pass. */
	}

	symbol_table_destroy(&sym_table);
	debug_list_destroy(&dbg_list);

	return 0;
}