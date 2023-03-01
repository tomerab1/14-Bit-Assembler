/** @file
*/

#include "utils.h"
#include "syntactical_analysis.h"
#include <string.h>
#include <ctype.h>

/**
* Reallocate memory and print error message if failed. This is a wrapper around realloc (). The function returns pointer to realloc ()'d memory.
* 
* @param ptr - Pointer to the memory to be reallocated.
* @param alloc_sz - Size of the memory to be real
*/
void* xrealloc(void* ptr, size_t alloc_sz)
{
    void* mem = realloc(ptr, alloc_sz);
    if (!mem) printf("%s\n", "Error: memory allocation failed !");
    return mem;
}

/**
* Allocate memory and return pointer to it. Print error message if calloc fails. Intended for use with XFree86_64 and XCEI_64
* 
* @param count - Number of bytes to allocate
* @param alloc_sz - Size of memory to
*/
void* xcalloc(size_t count, size_t alloc_sz)
{
    void* mem = calloc(count, alloc_sz);
    if (!mem) printf("%s\n", "Error: memory allocation failed !");
    return mem;
}

/**
* Allocate memory and return pointer to it. Print error message if failure. Intended to be used as a replacement for malloc ( 3 )
* 
* @param alloc_sz - Size of memory to
*/
void* xmalloc(size_t alloc_sz)
{
    void* mem = malloc(alloc_sz);
    if (!mem) printf("%s\n", "Error: memory allocation failed !");
    return mem;
}

/**
* Given a path and a postfix this function returns a new name that will be used as the output file name.
* 
* @param path
* @param postfix
* 
* @return A pointer to the new name or NULL if there was insufficient memory to allocate a new name. The caller must free this
*/
char* get_outfile_name(char* path, char* postfix)
{
    char* new_name = (char*)xcalloc(strlen(path) + strlen(postfix) + 1, sizeof(char));
    char* dot_loc = strrchr(path, POSTFIX_DOT_CHAR); /* Assuming path will contain a dot and then a postfix. */

    /* Will copy until the dot. */
    memmove(new_name, path, sizeof(char) * (dot_loc - path));
    strncat(new_name, postfix, strlen(postfix));

    return new_name;
}

/**
* Opens a file for reading. If there is an error the program exits with EXIT_FAILURE. This is a wrapper around fopen ( 3 )
* 
* @param path - The path to the file to open
* @param mode - The mode to open the file with
* 
* @return A pointer to the opened file or NULL on error. The caller must close the file when finished with
*/
FILE* open_file(char* path, char* mode)
{
    FILE* f = fopen(path, mode);

    if (!f) {
        printf("Error: Could not open %s for %s !\n", path, mode);
        exit(EXIT_FAILURE);
    }

    return f;
}

/**
* Read a line from a file. This is used to read the source file from the command line.
* 
* @param in - Pointer to the file to read from. Must be open for reading.
* 
* @return A pointer to the line read or NULL if there was no line to read from the file. The caller is responsible for freeing the buffer
*/
char* get_line(FILE* in)
{
    char* read_buffer = (char*)xcalloc(SOURCE_LINE_MAX_LENGTH + 1, sizeof(char));
    size_t read = 0;
    char ch;

    while ((ch = (char)fgetc(in)) != EOF && ch != '\n' && read < SOURCE_LINE_MAX_LENGTH) {
        read_buffer[read++] = (ch == TAB_CHAR) ? SPACE_CHAR : ch;
    }

    if (ch == '\n' && read == 0) {
        return read_buffer;
    }

    /* If nothing was read from the file */
    if (read == 0 && ch == EOF) {
        free(read_buffer);
        return NULL;
    }

    if (is_line_only_blanks(read_buffer)) {
        /* Mark for later functions in the pipeline to ignore. */
        *read_buffer = '\0';
        return read_buffer;
    }

    return read_buffer;
}

/**
* Checks if a line contains only blanks. This is used to prevent an attacker from trying to read a file that is too long to fit in a memory buffer.
* 
* @param line
* 
* @return TRUE if the line only contains blanks FALSE otherwise. Note that the return value is not guaranteed to be the same as is_line_blanks
*/
bool is_line_only_blanks(char* line)
{
    while (*line) {
        if (!isspace(*line)) 
            return FALSE;
        line++;
    }
    return TRUE;
}

/**
* Copy a string to a new memory block. The copy is allocated in xcalloc () so you must free it yourself before using it.
* 
* @param str
* 
* @return A pointer to the new memory block. It must be deallocated by xfree () when no longer needed
*/
char* get_copy_string(char* str)
{
    char* res = (char*)xcalloc(strlen(str) + 1, sizeof(char));
    memcpy(res, str, sizeof(char) * (strlen(str) + 1));
    return res;
}

/**
* Get number from string. This is used to parse numbers that are stored in the DB. The format is " %u "
* 
* @param num
*/
unsigned int get_num(char* num)
{
    int n;
    (void) sscanf(num, "%u", &n);
    return n;
}

/**
* Returns the 2's complement of an integer. This is equivalent to n^2 - 1 but works for integers less than 2^31
* 
* @param n - the integer to get the 2's complement
*/
unsigned int get_2s_complement(int n)
{
    return (~n) + 1;
}
