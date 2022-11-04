#include <stdio.h>
#include "pre_assembler.h"
#include "memory.h"

int main(int argc, char** argv)
{
	start_pre_assembler("SOURCE_FILE_TEST.TXT");
	
	CPU cpu = get_new_cpu();

	return 0;
}