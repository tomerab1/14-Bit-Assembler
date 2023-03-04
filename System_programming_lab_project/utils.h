#ifndef UTILS_H
#define UTILS_H

#define _CRT_SECURE_NO_WARNINGS

/** @file
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"

/*#define DEBUG*/

/* TODO: Remove later, just defined for visual studio. */
#ifndef __cplusplus
typedef unsigned short bool;
#endif

#define TRUE 1
#define FALSE 0

#define REG_MIN_NUM '0'
#define REG_MAX_NUM '7'
#define REG_BEG_CHAR 'r'
#define POS_SIGN_CHAR '+'
#define NEG_SIGN_CHAR '-'
#define HASH_CHAR '#'
#define COMMA_CHAR ','
#define COLON_CHAR ':'
#define OPEN_PAREN_CHAR '('
#define CLOSE_PAREN_CHAR ')'
#define SPACE_CHAR ' '
#define QUOTE_CHAR '\"'
#define TAB_CHAR '\t'

#define GROW_CAPACITY(old_sz) (old_sz) *= 2
#define GROW_ARRAY(type, pointer, new_count, size) (type)xrealloc((pointer), (new_count) * (size))
#define FREE_ARRAY(pointer) free((pointer))


/* Macro for iterating over each element of the list. */
#define LIST_FOR_EACH(type, head, out) \
type* (out);				\
for((out) = (head); (out) != NULL; (out) = (out) ? (out)->next : NULL)



/**
* @brief Reallocate memory and print error message if failed. This is a wrapper around realloc (). The function returns pointer to realloc ()'d memory.
*
* @param ptr - Pointer to the memory to be reallocated.
* @param alloc_sz - Size of the memory to be real
*/void* xrealloc(void* ptr, size_t alloc_sz);

/**
* @brief Allocate memory and return pointer to it. Print error message if calloc fails. Intended for use with XFree86_64 and XCEI_64
*
* @param count - Number of bytes to allocate
* @param alloc_sz - Size of memory to
*/
void* xcalloc(size_t count, size_t alloc_sz);

/**
* @brief Allocate memory and return pointer to it. Print error message if failure. Intended to be used as a replacement for malloc ( 3 )
*
* @param alloc_sz - Size of memory to
*/
void* xmalloc(size_t alloc_sz);

/**
* @brief Given a path and a postfix this function returns a new name that will be used as the output file name.
*
* @param path
* @param postfix
*
* @return A pointer to the new name or NULL if there was insufficient memory to allocate a new name. The caller must free this
*/
char* get_outfile_name(char* path, char* postfix);


/**
* @brief Opens a file for reading. If there is an error the program exits with EXIT_FAILURE. This is a wrapper around fopen ( 3 )
*
* @param path - The path to the file to open
* @param mode - The mode to open the file with
*
* @return A pointer to the opened file or NULL on error. The caller must close the file when finished with
*/
FILE* open_file(char* path, char* mode);


/**
* @brief Read a line from a file. This is used to read the source file from the command line.
*
* @param in - Pointer to the file to read from. Must be open for reading.
*
* @return A pointer to the line read or NULL if there was no line to read from the file. The caller is responsible for freeing the buffer
*/
char* get_line(FILE* in);

/**
* @brief Returns the 2's complement of an integer. This is equivalent to n^2 - 1 but works for integers less than 2^31
*
* @param n - the integer to get the 2's complement
*/
unsigned int get_2s_complement(int n);

/**
* @brief Copy a string to a new memory block. The copy is allocated in xcalloc () so you must free it yourself before using it.
*
* @param str
*
* @return A pointer to the new memory block. It must be deallocated by xfree () when no longer needed
*/
char* get_copy_string(char* str);

/**
* @brief Checks if a line contains only blanks. This is used to prevent an attacker from trying to read a file that is too long to fit in a memory buffer.
*
* @param line
*
* @return TRUE if the line only contains blanks FALSE otherwise. Note that the return value is not guaranteed to be the same as is_line_blanks
*/
bool is_line_only_blanks(char* line);

/**
* @brief Get number from string. This is used to parse numbers that are stored in the DB. The format is " %u "
*
* @param num
*/
unsigned int get_num(char* num);

#endif
