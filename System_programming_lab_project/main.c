#include <stdio.h>

#include "pre_assembler.h"
#include "memory.h"
#include "debug.h"
#include "first_pass.h"

#include "encoding.h"

int main(int argc, char** argv)
{
	start_pre_assembler("SOURCE_FILE_TEST.TXT");
	puts("[+] Pre-Assembler was done with no errors...");
	
	const char* pre_assembled_path = get_outfile_name("SOURCE_FILE_TEST.TXT", ".am");
	memoryBuffer img = memory_buffer_get_new();
	SymbolTable* sym_table = symbol_table_new_table();
	debugList* dbg_list = debug_list_new_list();

	puts("[!] Trying to do the first pass...");
	if (do_first_pass(pre_assembled_path, &img, sym_table, dbg_list)) {
		/* Create all the appropriate files, continue to second pass. */
		puts("[+] First pass was done with no errors...");
		puts("[!] Trying to do the second pass...");

		dump_memory(img);
	}

	puts("[-] First pass failed with these errors: ");
	debug_list_pretty_print(dbg_list);

	symbol_table_destroy(&sym_table);
	debug_list_destroy(&dbg_list);

	return 0;
}