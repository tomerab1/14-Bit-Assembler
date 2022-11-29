#ifndef UTILS_H
#define UTILS_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"

#define DEBUG

/* TODO: Remove later, just defined for visual studio. */
#ifndef __cplusplus
typedef unsigned short bool;
#endif

#define TRUE 1
#define FALSE 0

#define GROW_CAPACITY(old_sz) (old_sz) *= 2
#define GROW_ARRAY(type, pointer, new_count, size) (type)xrealloc((pointer), (new_count) * (size))
#define FREE_ARRAY(pointer) free((pointer))

/*<<<<<<< HEAD
=======*/
/* Macro for iterating over each element of the list. */
#define LIST_FOR_EACH(type, head, out) \
for(type* out = head; out != NULL; out = (out) ? out->next : NULL)
/*>>>>>>> master*/

/* Wrapper function of malloc, calloc and realloc, they check whether an allocation was successful or not. */
void* xrealloc(void* ptr, size_t alloc_sz);
void* xcalloc(size_t count, size_t alloc_sz);
void* xmalloc(size_t alloc_sz);

/* Creates a new file name, that end with 'postfix'. */
const char* get_outfile_name(const char* path, const char* postfix);

/* Opens a file for the specified I/O mode. */
FILE* open_file(const char* path, const char* mode);

/* Reads a new line from 'in'. */
char* get_line(FILE* in);

/* This function takes a negative value, and returns it's 2's complement form. */
unsigned int get_2s_complement(int n);

#endif