#ifndef LINE_ITERATOR_H
#define LINE_ITERATOR_H

#include "utils.h"

typedef struct
{
	char* start; // Pointer to the start of the line.
	char* current; // Pointer to the current position in the line.
} LineIterator;

/* Puts a new line into the iterator. */
void line_iterator_put_line(LineIterator* it, const char* line);

/* Advances the line iterator by 1. */
void line_iterator_advance(LineIterator* it);

/* Advances the line iterator by -1. */
void line_iterator_backwards(LineIterator* it);

/* Consumes blanks, i.e white spaces and tabs. */
void line_iterator_consume_blanks(LineIterator* it);

/* Returns the current char 'current' points to. */
char line_iterator_peek(LineIterator* it);

/* Checks if the iterator reached its end, i.e '\0'. */
bool line_iterator_is_end(LineIterator* it);

/*Checks if the iterator reached line beginning*/
bool line_iterator_is_start(LineIterator* it);
#endif
