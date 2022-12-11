#ifndef UTILS_H
#define UTILS_H

#define _CRT_SECURE_NO_WARNINGS

/** @file
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"

//#define DEBUG

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
for(type* (out) = (head); (out) != NULL; (out) = (out) ? (out)->next : NULL)


/* Wrapper function of malloc, calloc and realloc, they check whether an allocation was successful or not. */
void* xrealloc(void* ptr, size_t alloc_sz);
void* xcalloc(size_t count, size_t alloc_sz);
void* xmalloc(size_t alloc_sz);

/* Creates a new file name, that end with 'postfix'. */
char* get_outfile_name(char* path, char* postfix);

/* Opens a file for the specified I/O mode. */
FILE* open_file(char* path, char* mode);

/* Reads a new line from 'in'. */
char* get_line(FILE* in);

/* This function takes a negative value, and returns it's 2's complement form. */
unsigned int get_2s_complement(int n);

char* get_copy_string(char* str);

bool is_line_only_blanks(char* line);

unsigned int get_num(char* num);

#endif