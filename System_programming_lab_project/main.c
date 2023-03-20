#include "driver.h"

/*
	TODO:
	1. Add tests.
	2. Add memory leak logs.
	3. Complie again to see if there are no new warnings/errors.
	6. Refactor some of the code and add dispatch tables where possible.
	7. add the ability to get string like "abc"d".
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
