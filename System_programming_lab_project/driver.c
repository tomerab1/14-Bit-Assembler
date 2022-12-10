/** @file
*/

#include "driver.h"
#include "pre_assembler.h"
#include "memory.h"
#include "debug.h"
#include "first_pass.h"

typedef struct driver {
    SymbolTable* sym_table;
    debugList* dbg_list;
    memoryBuffer mem_buffer;
    int (*exec)(Driver* self, int argc, char** argv);
} Driver;

Driver* driver_new_driver()
{
    Driver* driver = (Driver*)xmalloc(sizeof(Driver));
    on_initialization(driver);
    return driver;
}

int driver_exec(Driver* driver, int argc, char** argv)
{
    return driver->exec(driver, argc, argv);
}

int exec_impl(Driver* driver, int argc, char** argv)
{
    int i;
    char* pre_assembler_path = NULL;

    for (i = 1; i < argc; i++) {
        start_pre_assembler(argv[i]);
        pre_assembler_path = get_outfile_name(argv[i], ".am");

        if (i > 1)
            on_initialization(driver);

        if (do_first_pass(pre_assembler_path, &driver->mem_buffer, driver->sym_table, driver->dbg_list)) {
            dump_memory(&driver->mem_buffer);
        }

        on_exit(driver);
        free(pre_assembler_path);
    }

    return 0;
}

void on_initialization(Driver* driver)
{
    driver->dbg_list = debug_list_new_list();
    driver->sym_table = symbol_table_new_table();
    driver->mem_buffer = memory_buffer_get_new();
    driver->exec = exec_impl;
}

void on_exit(Driver* driver)
{
    debug_list_destroy(&driver->dbg_list);
    symbol_table_destroy(&driver->sym_table);
}