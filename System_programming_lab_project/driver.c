/** @file
*/

#include "driver.h"
#include "pre_assembler.h"
#include "memory.h"
#include "debug.h"
#include "first_pass.h"
#include "second_pass.h"

typedef struct driver {
    SymbolTable* sym_table;
    debugList* dbg_list;
    memoryBuffer mem_buffer;
    int (*exec)(Driver* self, int argc, char** argv);
} Driver;

#define FIRST_PASS_FAILED 1
#define SECOND_PASS_FAILED 2

/**
* Create a new driver. This is called by libc's init_driver () to create a new driver.
* 
* 
* @return Pointer to the new driver or NULL if there was an error allocating memory. The caller must free the returned pointer
*/
Driver* driver_new_driver()
{
    Driver* driver = (Driver*)xmalloc(sizeof(Driver));
    driver->exec = exec_impl;
    on_initialization(driver);
    return driver;
}

/**
* Execute a driver. This is the entry point for drivers that wish to execute their command line interface.
* 
* @param driver
* @param argc - Number of arguments to pass to the driver.
* @param argv - Arguments to pass to the driver. These are passed as - is to the driver's exec () function.
* 
* @return 0 on success non - zero on failure. In this case the driver is uninitialized and must be set to NULL
*/
int driver_exec(Driver* driver, int argc, char** argv)
{
    return driver->exec(driver, argc, argv);
}

/**
* Executes the assembler. This is the entry point for the execution of the assembly. It will start the pre - assembler and do the first pass of initiating the second pass.
* 
* @param driver
* @param argc - Number of arguments to the assembly
* @param argv - Array of argv [ 0 ] to the assembly
* 
* @return 0 on success non - zero on failure ( in which case we're exiting the assembly without error
*/
int exec_impl(Driver* driver, int argc, char** argv)
{
    int i;
    char* pre_assembler_path = NULL;

    for (i = 1; i < argc; i++) {
        start_pre_assembler(argv[i]);
        pre_assembler_path = get_outfile_name(argv[i], ".am");

        if (i > 1) on_initialization(driver);
        if (!do_first_pass(pre_assembler_path, &driver->mem_buffer, driver->sym_table, driver->dbg_list)) debug_list_pretty_print(driver->dbg_list);
        if (!initiate_second_pass(pre_assembler_path, driver->sym_table, &driver->mem_buffer)) /* Errors */

        on_exit(driver);
        free(pre_assembler_path);
    }

    return 0;
}

/**
* Called when the driver is initialized. This is where we initialize the data structures that are used to store debug information.
* 
* @param driver
*/
void on_initialization(Driver* driver)
{
    driver->dbg_list = debug_list_new_list();
    driver->sym_table = symbol_table_new_table();
    driver->mem_buffer = memory_buffer_get_new();
}

/**
* Called when the module is unloaded. Destroys all memory allocated by the module. This is the last function called to clean up the module's data structures.
* 
* @param driver
*/
void on_exit(Driver* driver)
{
    debug_list_destroy(&driver->dbg_list);
    symbol_table_destroy(&driver->sym_table);
}
