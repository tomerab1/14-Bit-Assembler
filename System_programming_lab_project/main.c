#include "driver.h"

/** @file
*/

/*
	TODO:
	1. Write docs.
	2. Write data_img to the file.
	3. Fix encoding of labels.
*/

/**
* Entry point for the driver. This is the entry point for the driver to be executed.
* 
* @param argc - Number of command line arguments. If - 1 the program will use the standard argument parsing.
* @param argv - Arguments passed to the program. These are assumed to be UTF - 8 encoded strings.
* 
* @return 0 on success non - zero on failure. In this case an error is returned
*/
int main(int argc, char** argv)
{
	Driver* driver = driver_new_driver();
	return driver_exec(driver, argc, argv);
}