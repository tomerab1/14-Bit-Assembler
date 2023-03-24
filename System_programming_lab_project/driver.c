#include "driver.h"
#include "pre_assembler.h"
#include "memory.h"
#include "debug.h"
#include "first_pass.h"
#include "second_pass.h"

struct driver {
    SymbolTable* sym_table;
    memoryBuffer* mem_buffer;
};

#define FIRST_PASS_FAILED 1
#define SECOND_PASS_FAILED 2

Driver* driver_new_driver()
{
    Driver* driver = (Driver*)xmalloc(sizeof(Driver));
    return driver;
}

int driver_exec(Driver* driver, int argc, char** argv)
{
    return exec_impl(driver, argc, argv);
}

int exec_impl(Driver* driver, int argc, char** argv)
{
    int i;
    char* src_path = NULL, *pre_assembler_path = NULL;

    if (argc <= 1) {
	    printf("Usage: ./exe_name <files...>\n");
	    return 1;
    }

    for (i = 1; i < argc; i++) {
        src_path = get_outfile_name(argv[i], SRC_ASSEMBLER_FILE_EXTENSTION);
        start_pre_assembler(src_path);
        pre_assembler_path = get_outfile_name(src_path, PRE_ASSEMBLER_FILE_EXTENSTION);

        on_initialization(driver);

        /* If first pass failed returns false, otherwise returns true, goes as same for second pass */
        if (do_first_pass(pre_assembler_path, driver->mem_buffer, driver->sym_table))
            if (initiate_second_pass(pre_assembler_path, driver->sym_table, driver->mem_buffer))
                printf("\n~~~\nProcess completed successfully\n~~~\n");

        on_exit(driver);
        free(pre_assembler_path);
        free(src_path);
    }

    return 0;
}

void on_initialization(Driver* driver)
{
    driver->sym_table = symbol_table_new_table();
    driver->mem_buffer = memory_buffer_get_new();
}

void on_exit(Driver* driver)
{
    symbol_table_destroy(&driver->sym_table);
    memory_buffer_destroy(&driver->mem_buffer);
}

void driver_destroy(Driver** driver)
{
	free(*driver);
}

