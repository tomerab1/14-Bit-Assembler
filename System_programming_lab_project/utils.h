#ifndef UTILS_H
#define UTILS_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

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

void* xrealloc(void* ptr, size_t alloc_sz);
void* xcalloc(size_t count, size_t alloc_sz);
void* xmalloc(size_t alloc_sz);

/* Opens a file for the specified I/O mode. */
FILE* open_file(const char* path, const char* mode);

/* Reads a new line from 'in'. */
char* get_line(FILE* in);

#endif