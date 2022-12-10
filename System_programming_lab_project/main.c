#include "driver.h"

int main(int argc, char** argv)
{
	Driver* driver = driver_new_driver();
	return driver_exec(driver, argc, argv);
}