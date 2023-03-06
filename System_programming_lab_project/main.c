#include "driver.h"

/*
	TODO:
	1. Test memory leaks and build on Linux
	2. Replace final success message with something prettier
	3. add constants to everythin except for loop vars.
	4. finish add data abstraction to memory.h
	5. fix all memory leaks.
	6. run more tests.
	7. add more comments.
	8. free memoryBuffer.
	9. add utils.h to memory.o
*/

/**
* Entry point for the driver. This is the entry point for the driver to be executed.
* 
* @param argc - Number of command line arguments. If - 1 the program will use the standard argument parsing.
* @param argv - Arguments passed to the program.
* 
* @return 0 on success non - zero on failure
*/
int main(int argc, char** argv)
{
	int ret_val;
	Driver* driver = driver_new_driver();
	
	ret_val = driver_exec(driver, argc, argv);
	driver_destroy(&driver);

	return ret_val;
}
