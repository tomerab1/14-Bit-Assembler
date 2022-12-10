#ifndef DRIVER_H
#define DRIVER_H

/** @file
*/

/** @brief
 * Forward decleration for the programs driver. 
*/
typedef struct driver Driver;

/** Function for getting a new driver. 
 * @return A new pointer to a driver.
*/
Driver* driver_new_driver();

/** Function for executing the driver.
 * @param The driver to execute.
 * @param The mains argc
 * @param The mains argv
 * @return An appropriate exit code.
*/
int driver_exec(Driver* driver, int argc, char** argv);

/** Internal function for the execute function implementation.
 * @param The driver to execute.
 * @param The mains argc
 * @param The mains argv
 * @return An appropriate exit code.
*/
static int exec_impl(Driver* driver, int argc, char** argv);


/** Internal function for initializing the driver.
 * @param The driver to initialize.
*/
static void on_initialization(Driver* driver);

/** Internal function for releasing the driver's resources.
 * @param The driver to release.
*/
static void on_exit(Driver* driver);

#endif